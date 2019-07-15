/*
 * LED_definition.h
 *
 * Created: 2019-03-31 13:15:46
 *  Author: Kabina
 */ 


#ifndef LED_DEFINITION_H_
#define LED_DEFINITION_H_

#include <avr/io.h>

#define LED_PORT PORTC
#define B_LED    PC0
#define R_LED    PC1
#define G_LED    PC2
#define Y_LED    PC3
#define LED_DDR  DDRC
#define R_ON	 LED_PORT |= (1<<R_LED)
#define R_OFF	 LED_PORT &= ~(1<<R_LED)
#define G_ON	 LED_PORT |= (1<<G_LED)
#define G_OFF	 LED_PORT &= ~(1<<G_LED)
#define B_ON	 LED_PORT |= (1<<B_LED)
#define B_OFF	 LED_PORT &= ~(1<<B_LED)
#define Y_ON	 LED_PORT |= (1<<Y_LED)
#define Y_OFF	 LED_PORT &= ~(1<<Y_LED)


#endif /* LED_DEFINITION_H_ */