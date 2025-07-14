/*
 * hc05.c
 * Driver para módulo Bluetooth HC-05 com FreeRTOS
 * Autor: valld
 * Atualizado: 11/07/2025
 */

#include <avr/io.h>
#include <string.h>
#include "h05.h"
#include "freeRTOS/FreeRTOS.h"
#include "freeRTOS/queue.h"
#include "freeRTOS/task.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

#define TX_QUEUE_LEN 8
#define TX_MAX_LEN   128

static QueueHandle_t txQueue;

//inicializa o uart
void hc05_init(void) {
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)(UBRR_VALUE);

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   // Habilita TX e RX
    UCSR0C = (1 << UCSR0B)| (3 << UCSZ00);  // 8 bits, 1 stop
}

// enviar uma string
void hc05_send_string(const char *str) {
    while (*str != '\0') {
        while (!(UCSR0A & (1 << UDRE0)));  // Espera buffer livre
        UDR0 = *str++;
    }
}

// Envia string para a fila FreeRTOS (assíncrono)
char hc05_send_async(const char *str) {
    char buffer[TX_MAX_LEN];
    if (strlen(str) >= TX_MAX_LEN) return 0;

    strcpy(buffer, str);

    return xQueueSend(txQueue, buffer, 0) == pdPASS;
}


char hc05_read_char(){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void hc05_load(){
	hc05_send_string("OI");
	hc05_send_string("]\r\n");
}
/*
//Task do FreeRTOS que envia as strings da fila 
void vTaskBluetooth(void *pvParameters) {
    char msg[TX_MAX_LEN];
    while (1) {
        if (xQueueReceive(txQueue, &msg, portMAX_DELAY) == pdTRUE) {
            hc05_send_string(msg);
        }
    }
}
*/
