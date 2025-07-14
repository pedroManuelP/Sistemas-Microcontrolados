/*
 * CFile1.c
 *
 * Created: 11/07/2025 15:27:40
 *  Author: valld
 */ 

#ifndef HC05_H
#define HC05_H

void hc05_init(void); //inicializa o modulo com uart
void hc05_send_string(const char *str);// envia uma string de forma bloqueante a uart
char hc05_send_async(const char *str); // infileira uma string para o envio via bluetooth;
void vTaskBluetooth(void *pvParameters); //tarefa FreeRtos para enviar a meensagens via a fila
#endif
