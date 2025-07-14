#ifndef LCD_H
#define LCD_H
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

// LCD pin connections
#define LCD_RS     PD2
#define LCD_EN     PD3
#define LCD_D4     PD4
#define LCD_D5     PD5
#define LCD_D6     PD6
#define LCD_D7     PD7

#define LCD_PORT   PORTD
#define LCD_DDR    DDRD

// Macros
#define RS_HIGH()   (LCD_PORT |= (1 << LCD_RS))
#define RS_LOW()    (LCD_PORT &= ~(1 << LCD_RS))
#define EN_HIGH()   (LCD_PORT |= (1 << LCD_EN))
#define EN_LOW()    (LCD_PORT &= ~(1 << LCD_EN))

void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_clear(void);
void lcd_print(const char* str);
void lcd_goto(uint8_t row, uint8_t col);

#endif
