/*
 * Sterownik dwukana³owy LED
 *
 * Created: 2019-03-29 16:04:55
 *  Author: Krokodyl
 */ 



#include "LED_definition.h"
#include "2Channel RC LED DRIVER.h"
#include <util/delay.h>



void initPWM()
{
	CH_DDR |= (1<<OUTPUT_CH_1)|(1<<OUTPUT_CH_2); // piny steruj¹ce (PB1 PB2) jako wyjœcia
	TCCR1A |= (1 << COM1A1)|(1 << COM1B1)|(1 << COM1A0)|(1 << COM1B0); //inverting mode
	
	// FastPWM 10 bit
	TCCR1A |= (1 << WGM10)|(1<<WGM11);
	TCCR1B |= (1 << WGM12);
	
	// prescaler 1
	TCCR1B |= (1 << CS10); 
}

void checkCommand()
{
	switch (confirmedCommand)
	{
		case POWER:
					finalLevel_CH1 = LEV_100;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_1;
					break;
		case HIDE:
					finalLevel_CH2 = LEV_100;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_2;
					break;
		case RESYNC:
					finalLevel_CH1 = LEV_75;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_1;
					break;
		case FREEZE:
					finalLevel_CH2 = LEV_75;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_2;
					break;
		case VOL_PLUS:
					finalLevel_CH1 = LEV_50;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_1;
					break;
		case KEYSTONE_1:
					break;
		case D_ZOOM:
					finalLevel_CH2 = LEV_50;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_2;
					break;
		case VOL_MINUS:
					finalLevel_CH1 = LEV_25;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_1;
					break;
		case KEYSTONE_2:
					break;
		case DISPL_MODE:
					finalLevel_CH2 = LEV_25;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_2;
					break;
		case RESET:
					finalLevel_CH1 = LEV_0;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_1;
					break;
		case MENU:
					finalLevel_CH2 = LEV_0;
					dimStatus = PRESET_DIMM;
					selectedChannel = CH_2;
					break;
		case UP:
					dimStatus = MANUAL_DIMM;
					break;
		case DOWN:
					dimStatus = MANUAL_DIMM;
					break;
		case LEFT:
					selectedChannel = CH_1;
					break;
		case ENTER:
					selectedChannel = CH_1_AND_CH_2;
					break;
		case RIGHT:
					selectedChannel = CH_2;
					break;
		case COMPOSITE:
					finalLevel_CH1 = 0;
					finalLevel_CH2 = 0;
					dimStatus = ONOFF_DIMM;
					break;
		case S_VIDEO:
					finalLevel_CH1 = 0;
					finalLevel_CH2 = 0;
					dimStatus = ONOFF_DIMM;
					break;
		case NO_COMMAND:
					if(dimStatus==MANUAL_DIMM) dimStatus = NO_DIMM;
					break;
		
		default:
		break;
	}
}

void dimm()
{
	if (doDimmStepFlag_CH1)
	{
		doDimmStepFlag_CH1 = 0;
		if (dimStatus==MANUAL_DIMM && (selectedChannel == CH_1 || selectedChannel == CH_1_AND_CH_2))
		{
			if (actualCommand == DOWN && actualLevel_CH1>0) actualLevel_CH1--;
			if (actualCommand == UP && actualLevel_CH1<1023) actualLevel_CH1++;
		}
		if ((dimStatus==PRESET_DIMM && selectedChannel == CH_1) || dimStatus==ONOFF_DIMM)
		{
			if (actualLevel_CH1>finalLevel_CH1) actualLevel_CH1--;
			if (actualLevel_CH1<finalLevel_CH1) actualLevel_CH1++;
			if (dimStatus==PRESET_DIMM && actualLevel_CH1 == finalLevel_CH1) dimStatus = NO_DIMM;
		}
		
		// pobranie z tablicy OCR1_. Tablica zapisana w pamiêci programu (PROGMEM, pgm_read_word)
		OCR1A = pgm_read_word(&OCR_Array[actualLevel_CH1]);
	}
	
	if (doDimmStepFlag_CH2)
	{
		doDimmStepFlag_CH2 = 0;
		if (dimStatus==MANUAL_DIMM && (selectedChannel == CH_2 || selectedChannel == CH_1_AND_CH_2))
		{
			if (actualCommand == DOWN && actualLevel_CH2>0) actualLevel_CH2--;
			if (actualCommand == UP && actualLevel_CH2<1023) actualLevel_CH2++;
		}
		if ((dimStatus==PRESET_DIMM && selectedChannel == CH_2) || dimStatus==ONOFF_DIMM)
		{
			if (actualLevel_CH2>finalLevel_CH2) actualLevel_CH2--;
			if (actualLevel_CH2<finalLevel_CH2) actualLevel_CH2++;
			if (dimStatus==PRESET_DIMM && actualLevel_CH2 == finalLevel_CH2) dimStatus = NO_DIMM;
		}
		// pobranie z tablicy OCR1_
		OCR1B = pgm_read_word(&OCR_Array[actualLevel_CH2]);
	}
	
	if(dimStatus==ONOFF_DIMM && actualLevel_CH1==0 && actualLevel_CH2==0) dimStatus = NO_DIMM;
}

void calculateStep()
{
	if(dimStatus==MANUAL_DIMM)
	{
		dimmStepWidth_CH1 = 130;
		dimmStepWidth_CH2 = 130;
	}
	if (dimStatus==PRESET_DIMM)
	{
		if(finalLevel_CH1!=actualLevel_CH1) dimmStepWidth_CH1 = (uint32_t) FADE_TIME*2000/abs(finalLevel_CH1-actualLevel_CH1); // dla okresu Timera = 50us
		if(finalLevel_CH2!=actualLevel_CH2) dimmStepWidth_CH2 = (uint32_t) FADE_TIME*2000/abs(finalLevel_CH2-actualLevel_CH2);
	}	
	if (dimStatus==ONOFF_DIMM)
	{
		switch (actualCommand)
		{
			case COMPOSITE:
						if(actualLevel_CH1!=0) dimmStepWidth_CH1 = (uint32_t) ASLEEP_TIME_1*2000/actualLevel_CH1;
						if(actualLevel_CH2!=0) dimmStepWidth_CH2 = (uint32_t) ASLEEP_TIME_1*2000/actualLevel_CH2;
			break;
			case S_VIDEO:
						if(actualLevel_CH1!=0) dimmStepWidth_CH1 = (uint32_t) ASLEEP_TIME_2*2000/actualLevel_CH1;
						if(actualLevel_CH2!=0) dimmStepWidth_CH2 = (uint32_t) ASLEEP_TIME_2*2000/actualLevel_CH2;
			break;
		}
	}
	dimmTimeTick_CH1 = 0;
	dimmTimeTick_CH2 = 0;
}

int main(void)
{
	LED_DDR |= (1<<R_LED)|(1<<G_LED)|(1<<B_LED)|(1<<Y_LED);
	
	for (uint8_t i=0;i<8;i++)
	{
		R_ON;
		G_ON;
		B_ON;
		Y_ON;
		_delay_ms(30);
		R_OFF;
		G_OFF;
		B_OFF;
		Y_OFF;
		_delay_ms(30);
	}

	initPWM();
	initIrNEC();
	
	//************* Parametry pocz¹tkowe*******************
	OCR1A = pgm_read_word(&OCR_Array[0]);
	OCR1B = pgm_read_word(&OCR_Array[0]);
	actualCommand = NO_COMMAND;
	selectedChannel = CH_1;
	dimStatus = NO_DIMM;
	//*****************************************************
	
    while(1)
    {
		if (edge_Flag)
		{
			edge_Flag = 0;
			decodeNECframe();
		}
		
		// ******* Wykonywane jednorazowo po przyjêciu nowej komendy *****
		if (confirmedCommand != actualCommand)
		{
			actualCommand = confirmedCommand;
			checkCommand();
			calculateStep();
		}
		//****************************************************************
		
		
		if (dimStatus!=NO_DIMM) dimm();
		
		
		//test------------------------------------------------
		if (dimStatus==MANUAL_DIMM)//manualDimmFlag)
		{
			Y_ON;
		} 
		
		if (dimStatus==PRESET_DIMM || dimStatus==ONOFF_DIMM)
		{
			B_ON;
		}
		if (dimStatus==NO_DIMM)//manualDimmFlag)
		{
			Y_OFF;
			B_OFF;
		}
		//-------------------------------------------------------
    }
}