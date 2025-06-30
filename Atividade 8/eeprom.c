#include "eeprom.h"


void writeEEPROM(unsigned int addr, int8_t dados){
    while (EECR & (1 << EEPE));
    EEAR = addr;
    EEDR = dados;
    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}
int8_t readEEPROM(unsigned int addr){
    while (EECR & (1 << EEPE));

    EEAR = addr;

    EECR |= (1 << EERE);
return EEDR;
}

void write16EEPROM(unsigned int addr, int16_t dados){
    writeEEPROM(addr, dados & 0xFF);
    writeEEPROM(addr+1, (dados>>8) & 0xFF);
}
int16_t read16EEPROM(unsigned int addr){
    int8_t LSBYTE = readEEPROM(addr);
    int8_t MSBYTE = readEEPROM(addr+1);
    return (int16_t) ((MSBYTE<<8) | LSBYTE);
}

void saveEEPROM(volatile int16_t cfs[]){

    write16EEPROM(C0addr,  cfs[0]);
    write16EEPROM(C1addr,  cfs[1]);
    write16EEPROM(C2addr,  cfs[2]);
    write16EEPROM(C3addr,  cfs[3]);
    write16EEPROM(C4addr,  cfs[4]);
    write16EEPROM(C5addr,  cfs[5]);
    write16EEPROM(C6addr,  cfs[6]);
    write16EEPROM(C7addr,  cfs[7]);
    write16EEPROM(C8addr,  cfs[8]);
    write16EEPROM(C9addr,  cfs[9]);
    write16EEPROM(C10addr, cfs[10]);
    write16EEPROM(C11addr, cfs[11]);
    write16EEPROM(C12addr, cfs[12]);
    write16EEPROM(C13addr, cfs[13]);
    write16EEPROM(C14addr, cfs[14]);
    write16EEPROM(C15addr, cfs[15]);

}

void loadEEPROM(volatile int16_t cfs[]){

    cfs[0]  = read16EEPROM(C0addr);
    cfs[1]  = read16EEPROM(C1addr);
    cfs[2]  = read16EEPROM(C2addr);
    cfs[3]  = read16EEPROM(C3addr);
    cfs[4]  = read16EEPROM(C4addr);
    cfs[5]  = read16EEPROM(C5addr);
    cfs[6]  = read16EEPROM(C6addr);
    cfs[7]  = read16EEPROM(C7addr);
    cfs[8]  = read16EEPROM(C8addr);
    cfs[9]  = read16EEPROM(C9addr);
    cfs[10] = read16EEPROM(C10addr);
    cfs[11] = read16EEPROM(C11addr);
    cfs[12] = read16EEPROM(C12addr);
    cfs[13] = read16EEPROM(C13addr);
    cfs[14] = read16EEPROM(C14addr);
    cfs[15] = read16EEPROM(C15addr);

}

void writeFloatEEPROM(unsigned int addr, float value) {
    uint8_t *ptr = (uint8_t*) &value;
    for (uint8_t i = 0; i < 4; i++) {
        writeEEPROM(addr + i, ptr[i]);
    }
}

float readFloatEEPROM(unsigned int addr) {
    uint8_t bytes[4];
    for (uint8_t i = 0; i < 4; i++) {
        bytes[i] = readEEPROM(addr + i);
    }

    float result;
    uint8_t *ptr = (uint8_t*) &result;
    for (uint8_t i = 0; i < 4; i++) {
        ptr[i] = bytes[i];
    }

    return result;
}
