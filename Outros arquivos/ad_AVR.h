#ifndef AD_AVR_H
#define AD_AVR_H

void ad_init();
uint16_t ad_get(uint8_t canal);

void ad_init(){
	clr_bit(PRR,PRADC);
	ADMUX = 0x40;//REFS1:0 = tensão de referencia Mux3:1 = sinal no conversor
	ADCSRA = 0x07;//ADPS2:0 = pre-escala do clk
	ADCSRB = 0x00;//ADTS2:0 = trigger mode
	set_bit(ADCSRA,ADEN);//habilita AD
}

uint16_t ad_get(uint8_t canal){
	ADMUX = 0x40 | canal;
	set_bit(ADCSRA,ADSC);//inicia conversão
	while (!(ADCSRA & (1 << ADIF)));//espera conversão acabar
	return ADC;
}

#endif