/*
 * irNEC.c
 *
 * Created: 2019-03-31 12:05:08
 *  Author: Krokodyl
 */ 

#include "irNEC.h"
#include "LED_definition.h"
#include "Optoma_BR3020N_Data.h"


//enum states IrState;
static volatile enum states{ STANDBY, LEAD_CHECKING, READING_DATA, WAIT_500ms, WAIT, REPEAT_CHECKING, ZAJEZDNIA } IrState;
commandDescryption *commDescPntr;
	

 void setFallingEdgeInterrupt(void)
{
	EICRA &= ~(1<<ISC00); //ustaw wykrywanie zbocza opadaj¹cego
}

 void setRisingEdgeInterrupt(void)
{
	EICRA |= (1<<ISC00);  //ustaw wykrywanie zbocza narastaj¹cego
}

 void toggleEdgeInterrupt(void)
{
	//ustaw wykrywanie zbocza przeciwnego do aktualnie ustawionego
	EICRA ^= (1<<ISC00);
	
}



void initIrNEC ()
{
	edge_Flag = 0;
	edgeDirection = FALLING;// ?
	runningTime = 0;
	transmissionTime = 0;
	measuredTime = 0;
	commandsTableSize = sizeof(commandsTable)/sizeof(commandsTable[0]);
	
	initTimer2_50us();
	initINT0();
	resetFrameReading();
	sei();
} //initIrNEC ()


void initTimer2_50us()
{
	//setup timer for 50us
	TCCR2A = (1 << WGM21); // CTC
	TCCR2B =  (1 << CS21); // Preskaler 8
	TIMSK2 = (1 << OCIE2A); // Compare Match A enabled
	OCR2A = (F_CPU/(20000L*8))-1; //50us
} //initTimer2_50us()


void initINT0()
{
	IrDDR  &= ~(1<<Ir); // Jako wejœcie
	IrPort &= ~(1<<Ir); // Bez podci¹gniêtego rezystora
	
	EICRA |= (1<<ISC01); //wykrywanie zbocza opadaj¹cego
	EIMSK  |= (1<<INT0); // W³¹cz przerwanie INT0
} //initINT0()


 ISR(TIMER2_COMPA_vect)
{
	runningTime++;
 	transmissionTime++;
	 
	 
	//********** potrzebne tylko do œciemniania (niepotrzebne dla dzia³ania NEC) ***********
	//***************************************************************************************
	dimmTimeTick_CH1++; 
	if (dimmTimeTick_CH1 == dimmStepWidth_CH1)
	{
		dimmTimeTick_CH1 = 0;
		doDimmStepFlag_CH1 = 1;
	}
	dimmTimeTick_CH2++;
	if (dimmTimeTick_CH2 == dimmStepWidth_CH2)
	{
		dimmTimeTick_CH2 = 0;
		doDimmStepFlag_CH2 = 1;
	}
	//***************************************************************************************
	//***************************************************************************************
	
	
	 if (IrState == WAIT_500ms)
	 {
		 if(transmissionTime == TIME_MAX_500ms) // zerowanie ramki w przypadku zakoñczenia transmisji
		 {
			 resetFrameReading();
		 }
	 }
	 
	 if (IrState == WAIT)
	 {
		 if(transmissionTime == TIME_INT0_RESTORE_108ms)  //TODO - opis
		 {
			 edge_Flag = 0;
			 allowEdgeFlag = 1;
		 }
		 if(transmissionTime == TIME_MAX_108ms) // zerowanie ramki w przypadku zakoñczenia transmisji
		 {
			 resetFrameReading();
		 }
	 }
}

ISR(INT0_vect)
{
	edgeDirection = EICRA & (1<<ISC00); // Przypisanie edgeDirection wartoœci bitu ICS00 (0 - przerwanie wprzy zboczu opadaj¹cym, 1 - przerwanie przy zboczu narastaj¹cym)
	if(allowEdgeFlag)
	{
		toggleEdgeInterrupt(); // po odczytaniu zbocza prze³¹cz w oczekiwanie zbocza przeciwnego
		edge_Flag = 1; // ustaw flagê obserwowan¹ w pêtli g³ównej programu
	}
	
}

//************************* funkcja odczytu ramki ********************
//********************************************************************
void decodeNECframe()
{
	switch (IrState)
	{
		case ZAJEZDNIA:
		break;

		case STANDBY:
					runningTime = 0; // zacznij mierzyæ d³ugoœæ pierwszego impulsu
					transmissionTime = 0; // zacznij mierzyæ d³ugoœæ ramki
					IrState = LEAD_CHECKING; // przejd¿ do fazy LEAD_CHECKING (przy nastêpnym zboczu - break)
					break;
			
		case LEAD_CHECKING:
					switch (edgeDirection)
					{
						case RISING:
								measuredTime = runningTime; //odczytaj czas pierwszego impulsu
								runningTime = 0; // zacznij mierzyæ drugi impuls
								
								// jeœli czas ipulsu poza zakresem 8,5-9,5 ms. zeruj odczyt
								// jeœli w zakresie, kontynuuj - czekaj na kolejne zbocze (opadaj¹ce)
								if (measuredTime<(TIME_LEAD_1-TIME_TOLERANCE_LEAD) || measuredTime>(TIME_LEAD_1+TIME_TOLERANCE_LEAD)) 
								{
									resetFrameReading();
									return;
								}
								else //TODO 
								{
								}
								break;
				
						case FALLING: 
								measuredTime = runningTime; //odczytaj czas drugiego impulsu
								
								// jeœli czas ipulsu poza zakresem 8,5-9,5 ms. zeruj odczyt
								// jeœli w zakresie, przejdz do fazy READING_DATA
								if (measuredTime<(TIME_LEAD_2-TIME_TOLERANCE_LEAD) || measuredTime>(TIME_LEAD_2+TIME_TOLERANCE_LEAD)) 
								{
									resetFrameReading();
									return;
								}
								else 
								{
									IrState = READING_DATA;
								}
								break;
					}					
					break;
		
		case READING_DATA:
			switch (edgeDirection)
			{
				//-----------
				case RISING:
					runningTime = 0;					
				break;
				
				//-----------
				case FALLING:
					measuredTime = runningTime;
					// Jeœli czas impulsu = 1,6 ms. (+/- toler.) zapisz jedynkê
					if ((measuredTime>(TIME_ONE-TIME_TOLERANCE_ONES)) && (measuredTime<(TIME_ONE+TIME_TOLERANCE_ONES)))
					{
						// zapisz "1" do bitu o numerze = bitIndex
						 frameBytes[byteIndex] |= (1<<(bitIndex));
					}
					
					if (bitIndex==0)
					{
						// Potwierdzenie bajtów adresu 
						if (byteIndex == ADDRESS_LB_INDEX && !(REMOTE_ADDRESS_LB == frameBytes[ADDRESS_LB_INDEX]))
						{
							resetFrameReading();
							return;
						}
						if (byteIndex == ADDRESS_HB_INDEX && !(REMOTE_ADDRESS_HB == frameBytes[ADDRESS_HB_INDEX]))
						{
							resetFrameReading();
							return;
						}
						
						if (byteIndex == INV_COMND_BYTE_INDEX)
						{
							//TODO
							// Sprawdzenie Bajtów komendy
							//if(!(frameBytes[INV_COMND_BYTE_INDEX] ^ frameBytes[COMMAND_BYTE_INDEX]))
							//{
								//resetFrameReading();
								//return;
							//}
							
							// Wyszukanie komendy pilota, której odpowiada odebrany bajt komendy i przypisanie jej do zmiennej
							
							for (uint8_t i=0; i<commandsTableSize;i++)
							{
								if (commandsTable[i].commandValue == frameBytes[COMMAND_BYTE_INDEX])
								{
									confirmedCommand = commandsTable[i].action;
									
									IrState = WAIT_500ms; // Tylko dla Optoma. Normalnie IrState = WAIT;
									return;
								}
							}
							
						}
												
						bitIndex = 7;
						byteIndex++;
					}
					else
					{
						bitIndex--;
					}
				break;
			}
		break;
			
//----------------	


		case WAIT_500ms:
	
			switch(edgeDirection)
			{
				case RISING:
				break;
				
				case FALLING:
				runningTime = 0;
				transmissionTime = 0;
				IrState = REPEAT_CHECKING;
				break;
			}
		
		break;	
		
		
		case WAIT:
				runningTime = 0;
				transmissionTime = 0;
				IrState = REPEAT_CHECKING;
		break;
		
		case REPEAT_CHECKING:
				measuredTime = runningTime;
				runningTime = 0;
				if (measuredTime<(TIME_LEAD_1-TIME_TOLERANCE_LEAD) || measuredTime>(TIME_LEAD_1+TIME_TOLERANCE_LEAD)) // poza 8,5-9,5 ms.
				{
					resetFrameReading();
					return;
				}
				else
				{
					allowEdgeFlag = 0;
					IrState = WAIT;
				}
		break;
	}
}

void resetFrameReading ()
{ 
	IrState = STANDBY;
	bitIndex = 7;
	byteIndex = ADDRESS_LB_INDEX;
	confirmedCommand = NO_COMMAND;
	EICRA |= (1<<ISC01); //wykrywanie zbocza opadaj¹cego
	EIMSK  |= (1<<INT0); // W³¹cz przerwanie INT0
	allowEdgeFlag = 1;
	
	// Zerowanie bajtów ramki
	for (uint8_t i=0;i<4;i++)
	{
		frameBytes[i] = 0x00;
	}
}


