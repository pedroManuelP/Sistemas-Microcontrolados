#ifndef EEPROM_H
#define EEPROM_H
#include <avr/io.h>
#include "eepromaddr.h"



void writeEEPROM(unsigned int addr, int8_t dados);
int8_t readEEPROM(unsigned int addr);
void write16EEPROM(unsigned int addr, int16_t dados);
int16_t read16EEPROM(unsigned int addr);
void writeFloatEEPROM(unsigned int addr, float value);
float readFloatEEPROM(unsigned int addr);

void saveEEPROM(volatile int16_t cfs[]);
void loadEEPROM(volatile int16_t cfs[]);
#endif