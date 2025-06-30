#include "lcd.h"
#define F_CPU 16000000UL

static void lcd_pulse_enable(void) {
    EN_HIGH();
    _delay_us(1);
    EN_LOW();
    _delay_us(100);
}

static void lcd_write_4bits(uint8_t nibble) {
    // Clear the data bits first (PD4–PD7)
    LCD_PORT &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7));

    // Set the new data bits
    if (nibble & 0x01) LCD_PORT |= (1 << LCD_D4);
    if (nibble & 0x02) LCD_PORT |= (1 << LCD_D5);
    if (nibble & 0x04) LCD_PORT |= (1 << LCD_D6);
    if (nibble & 0x08) LCD_PORT |= (1 << LCD_D7);

    lcd_pulse_enable();
}

static void lcd_send(uint8_t value, uint8_t is_data) {
    if (is_data)
        RS_HIGH();
    else
        RS_LOW();

    lcd_write_4bits(value >> 4); // High nibble
    lcd_write_4bits(value & 0x0F); // Low nibble
    _delay_ms(2);
}

void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
}

void lcd_data(uint8_t data) {
    lcd_send(data, 1);
}

void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_print(const char* str) {
    while (*str) {
        lcd_data(*str++);
    }
}

void lcd_goto(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 + col : 0xC0 + col;
    lcd_command(address);
}

void lcd_init(void) {
    LCD_DDR |= (1 << LCD_RS) | (1 << LCD_EN) |
               (1 << LCD_D4) | (1 << LCD_D5) |
               (1 << LCD_D6) | (1 << LCD_D7);

    _delay_ms(40); // Wait after power on

    // Initialization in 4-bit mode
    lcd_write_4bits(0x03);
    _delay_ms(5);
    lcd_write_4bits(0x03);
    _delay_us(150);
    lcd_write_4bits(0x03);
    lcd_write_4bits(0x02); // Set 4-bit mode

    lcd_command(0x28); // Function set: 4-bit, 2 line, 5x8 dots
    lcd_command(0x08); // Display OFF
    lcd_command(0x01); // Clear display
    lcd_command(0x06); // Entry mode set
    lcd_command(0x0C); // Display ON, Cursor OFF
}
