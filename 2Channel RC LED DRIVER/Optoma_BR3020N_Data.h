/*
 * Optoma_BR3020N_Data.h
 *
 * Created: 2019-04-12 12:07:06
 *  Author: krokodyl
 */ 


#ifndef OPTOMA_BR3020N_DATA_H_
#define OPTOMA_BR3020N_DATA_H_

//Odczyt z oscyloskopu: 11110010|00001010 (LowB|HIghB).
#define REMOTE_ADDRESS_LB 0xF2  
#define REMOTE_ADDRESS_HB 0xA

commandDescryption commandsTable[] =
{
	{0xFF, NO_COMMAND}, 
	{0xC8, UP}, // POWTÓRZENIA
	{0x28, DOWN}, // POWTÓRZENIA
	{0x40, POWER},
	{0xC0, HIDE},
	{0x20, RESYNC},
	{0x30, FREEZE},
	{0xE0, VOL_PLUS}, // POWTÓRZENIA
	{0xA0, D_ZOOM},
	{0x50, VOL_MINUS}, // POWTÓRZENIA
	{0x60, DISPL_MODE},
	{0xB0, RESET},
	{0x88, MENU},
	{0x10, KEYSTONE_1}, // POWTÓRZENIA
	{0xD0, KEYSTONE_2}, // POWTÓRZENIA
	{0x68, LEFT}, // POWTÓRZENIA
	{0xE8, ENTER}, // POWTÓRZENIA
	{0xA8, RIGHT},
	{0xD8, VGA_1},
	{0x38, COMPOSITE},
	{0xB8, S_VIDEO},
	{0x78, VGA_2}
};



#endif /* OPTOMA_BR3020N_DATA_H_ */