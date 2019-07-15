/*
 * _2Channel_RC_LED_DRIVER.h
 *
 * Created: 2019-07-02 11:38:40
 *  Author: krokodyl
 */ 


#ifndef TWOCHANNEL_RC_LED_DRIVER_H_
#define TWOCHANNEL_RC_LED_DRIVER_H_


#include <avr/pgmspace.h>
#include <stdlib.h>
#include "irNEC.h"
#include "lev_Array.h"

#define CH_PORT PORTB
#define OUTPUT_CH_1    PB1
#define OUTPUT_CH_2    PB2
#define CH_DDR  DDRB

#define FADE_TIME 6 //czas przejœcia z jednego poziamu do drugiego (1 = 1/10 sek)
#define ASLEEP_TIME_1 600
#define ASLEEP_TIME_2 6000

#define CH_1 1
#define CH_2 2
#define CH_1_AND_CH_2 3

#define LEV_0 0
#define LEV_25 20
#define LEV_50 250
#define LEV_75 520
#define LEV_100 1023

#define NO_DIMM 0
#define MANUAL_DIMM 1
#define PRESET_DIMM 2
#define ONOFF_DIMM 3


uint16_t actualLevel_CH1;
uint16_t finalLevel_CH1;
uint16_t actualLevel_CH2;
uint16_t finalLevel_CH2;
uint8_t actualCommand;
uint8_t manualDimmFlag;
uint8_t autoDimmFlag;
uint8_t longDimmFlag;
uint8_t selectedChannel;
uint8_t dimStatus;

void initPWM ();
void checkCommand ();
void dimm ();
void calculateStep();



#endif /* 2CHANNEL RC LED DRIVER_H_ */