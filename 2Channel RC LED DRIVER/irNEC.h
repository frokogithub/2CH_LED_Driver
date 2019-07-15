/*
 * irNEC.h
 *
 * Created: 2019-03-31 11:52:36
 *  Author: Krokodyl
 *
 */ 


#ifndef IRNEC_H_
#define IRNEC_H_

	#define F_CPU 16000000L

	#include <inttypes.h>
	#include <avr/io.h>
	#include <avr/interrupt.h>

	#define IrPort PORTD
	#define Ir     PD2
	#define IrDDR  DDRD
	
	#define FALLING 0
	#define RISING  1
	
	
	// Czasy w ramce (1ms = 20)
	#define TIME_LEAD_1 180 // 9 ms
	#define TIME_LEAD_2 90 // 4,5 ms
	//#define TIME_LEAD_2_REPEAT 45 // 2,25 ms
	#define TIME_ONE 32 // 1,6 ms
	#define TIME_TOLERANCE_ONES 2 // 0,1 ms
	#define TIME_TOLERANCE_LEAD 10 // 0,5 ms
	#define TIME_INT0_RESTORE_500ms 9920 // 496 ms
	#define TIME_INT0_RESTORE_108ms 2120 // 106 ms 
	#define TIME_MAX_500ms 10100 // 505 ms 
	#define TIME_MAX_108ms 2240 // 112 ms
	
	volatile uint8_t edge_Flag;// =0;
	volatile uint16_t runningTime;// = 0;
	uint16_t measuredTime;
	volatile uint8_t edgeDirection;// = 1; // 0-przerwanie przy opadajacym, 1-przerw. przy narastaj¹cym
	uint8_t  bitIndex; // numer odczytywanego bitu
	uint8_t  byteIndex; //numer odczytywanego bajtu (0 - adres LB, 1 - adres HB, 2 - command data, 3- inversed command data)
	uint8_t commandsTableSize;
	volatile uint16_t transmissionTime;
	volatile uint8_t allowEdgeFlag;
	
	
	//**** zmienne wykorzystane do œciemniania (niepotrzebne dla dzia³ania NEC) *************
	//***************************************************************************************
	volatile uint32_t dimmTimeTick_CH1;
	volatile uint32_t dimmStepWidth_CH1;
	volatile uint8_t doDimmStepFlag_CH1;
	volatile uint32_t dimmTimeTick_CH2;
	volatile uint32_t dimmStepWidth_CH2;
	volatile uint8_t doDimmStepFlag_CH2;
	//***************************************************************************************
	//***************************************************************************************
	
	
	uint8_t frameBytes [4]; // Tablica bajtów ramki
	#define ADDRESS_LB_INDEX 0
	#define ADDRESS_HB_INDEX 1
	#define COMMAND_BYTE_INDEX 2
	#define INV_COMND_BYTE_INDEX 3
	
	//-- Tworzê nowy typ zmiennej - typ commandName
	//enum actions for all remotes
	typedef enum {NO_COMMAND, UP, DOWN, POWER, HIDE, RESYNC, FREEZE, VOL_PLUS, D_ZOOM, VOL_MINUS, DISPL_MODE, 
				  RESET, MENU, KEYSTONE_1, KEYSTONE_2, LEFT, ENTER, RIGHT, VGA_1, COMPOSITE, S_VIDEO, VGA_2} commandName;
	
	commandName confirmedCommand;
	
	//-- Tworzê nowy typ zmiennej - typ commandTemplate jako strukturê z³o¿on¹ z pary commandValue i commandName
	typedef struct
	{
		uint8_t commandValue;
		commandName action;
	} commandDescryption;
	
	
	
	
	void setFallingEdgeInterrupt(void);
	void setRisingEdgeInterrupt(void);
	void toggleEdgeInterrupt(void);
	//void leadPulseChecking(uint8_t);
	void decodeNECframe(void);
	void resetFrameReading (void);
	void dataValidation(void);
	void initIrNEC (void);
	void initTimer2_50us(void);
	void initINT0 (void);
	//void temp (void);

#endif /* IRNEC_H_ */