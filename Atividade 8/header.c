#include "header.h"


void writeHeader(short int type, short int duty_cycle, short int status, short int freq, float amplitude, float offset){
    //0 = quadrada, 1= triang, 2=rampa,3=sen
    char offset_s[7];
    char duty_cycle_s[6];
    char amplitude_s[7];
    char freq_s[6];
    dtostrf(offset, 5, 1, offset_s);
    dtostrf(amplitude, 5, 1, amplitude_s);
    snprintf(duty_cycle_s, sizeof(duty_cycle_s), "%d",duty_cycle);
    snprintf(freq_s, sizeof(freq_s), "%d",freq);
    switch (type)
    {
    case quadrada_t:
        lcd_goto(0,0);
        lcd_print("T:QUA ");
        lcd_print("D:");
        lcd_print(duty_cycle_s);
        lcd_print("%   ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");

        break;
    case triangular_t:
        lcd_goto(0,0);
        lcd_print("T:TRI ");
        lcd_print("D:");
        lcd_print(duty_cycle_s);
        lcd_print("%   ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    case rampa_t:
        lcd_goto(0,0);
        lcd_print("T:RAM        ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    case sen_t:
        lcd_goto(0,0);
        lcd_print("T:SEN        ");
        if(status) lcd_print("ON");
        else lcd_print("OFF");
        lcd_goto(1,0);
        lcd_print(freq_s);
        lcd_print("Hz");
        lcd_print(amplitude_s);
        lcd_print("V");
        lcd_print(offset_s);
        lcd_print("V");
    break;
    default:
        break;
    }
}

void updateVars(volatile short int type, volatile short int dutycycle, volatile short int status, volatile short int frequencia, volatile float amplitude, volatile float offset){
    bool validType = (type >= 0 && type < 4);
    
  
    bool validDutyCycle = (dutycycle > 0 && dutycycle < 100);
    

    bool validStatus = (status == 0 || status == 1);
    
    bool validFreq = (frequencia > 0 && frequencia <= 100);

    bool validAmplitude = (amplitude >= 0.0f);
    
   
    bool validVoltage = ((offset + amplitude) <= 5.0f) && 
                       ((offset - amplitude) >= 0.0f);
    
    if (validType && validDutyCycle && validStatus && 
        validFreq && validAmplitude && validVoltage) {
        gType = type;
        gDutyCycle = dutycycle;
        gstatus = status;
        gfreq = frequencia;
        gamplitude = amplitude;
        goffset = offset;
    }
}