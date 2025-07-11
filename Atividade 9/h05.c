/*
 * CFile2.c
 *
 * Created: 11/07/2025 15:31:56
 *  Author: valld
 */ 

#include "hc5.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//Mapeando todos os enderecos dos registrados UART0

#define UBRR0H (*(volatile unsigned char *) 0xC5)
#define UBRROL (*(volatile unsigned char *) 0xC4)
#define UCRSR0C (*(volatile unsigned char *) 0xC2)
#define UCRSR0B (*(volatile unsigned char *) 0xC1)
#define UCRSR0A (*(volatile unsigned char *) 0xC0)
#define UDR0 (*(volatile unsigned char *) 0xC6)


// setando os bits de controle

#define TXEN0 3
#define UDRE0 5
#define UCSZ00 1
#define UCSZ01 12

// setando os tempos de clocks

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) -1)

// configuração de controle

#define TX_QUEUE_LEN 8
#define TX_MAX_LEN 128

static QueueHandle_t txQueue;

// funcoes auxiliares uma vez que não pode utilziar string.h

int str_len(const char *s) {
	int i = 0;
	while (s[i] != '\0'){
		i++;
	}
	
	return i;
}

void str_copy( char *dest, const char *src){
	int i = 0;
	while ((dest[i] = src[i]) != '\0'){
		i++;
	}
}


//inicializa usart

void hc05_init(void){
	UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
	UBRROL = (unsigned char)(UBRR_VALUE);
	
	UCRSR0B = (1 << TXEN0);//habilitando tx
	UCRSR0C = (1 << UCSZ01) | (1 << UCSZ00); // habilitando 8bits
	
	txQueue = xQueueCreate(TX_QUEUE_LEN, TX_MAX_LEN);
}

// envia a string pela UART (bloqueado)
void hc05_send_string(const char *str){
	int i = 0;
	while(str[i] != '\0'){
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = str[i++];
	}
}

//envia a string pela UART para envio assinrono 

void hc05_send_async(const char *str){
	char buffer[TX_MAX_LEN];
	
	if(str_len(str) >= TX_MAX_LEN) return 0;
	
	str_copy(buffer,str);
	
	if(xQueueSend(txQueue, &buffer, 0) == pdPass) return 1;
	else return 0;
}

void vTaskBluetooth(void *pvParameters){
	char msg[TX_MAX_LEN];
	
	while(1){
		if (xQueueReceive(txQueue, &msg, portMAX_DELAY) == pdTRUE) hc05_send_string(msg);
	}
}