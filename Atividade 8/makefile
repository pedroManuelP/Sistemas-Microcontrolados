FILENAME   = at08
PORT       = /dev/ttyACM0
DEVICE     = atmega328p
PROGRAMMER = arduino
BAUD       = 115200
COMPILE    = avr-gcc -Wall -Os -mmcu=$(DEVICE)

default: compile upload clean

compile:
	$(COMPILE) -c $(FILENAME).c -o $(FILENAME).o
	$(COMPILE) -c lcd.c -o lcd.o
	$(COMPILE) -c eeprom.c -o eeprom.o
	$(COMPILE) -c dac_8bit.c -o dac_8bit.o
	$(COMPILE) -c adc.c -o adc.o
	$(COMPILE) -c lookuptriangular.c -o lookuptriangular.o
	$(COMPILE) -c lookuprampa.c -o lookuprampa.o
	$(COMPILE) -c lookupsquare.c -o lookupsquare.o
	$(COMPILE) -c lookupseno.c -o lookupseno.o
	$(COMPILE) -c freertos/event_groups.c -o freertos/event_groups.o
	$(COMPILE) -c freertos/heap_3.c -o freertos/heap_3.o
	$(COMPILE) -c freertos/hooks.c -o freertos/hooks.o

	$(COMPILE) -c freertos/list.c -o freertos/list.o
	$(COMPILE) -c freertos/port.c -o freertos/port.o
	$(COMPILE) -c freertos/queue.c -o freertos/queue.o
	$(COMPILE) -c freertos/stream_buffer.c -o freertos/stream_buffer.o
	$(COMPILE) -c freertos/tasks.c -o freertos/tasks.o
	$(COMPILE) -c freertos/timers.c -o freertos/timers.o
	$(COMPILE) -o $(FILENAME).elf $(FILENAME).o lcd.o eeprom.o dac_8bit.o adc.o lookuprampa.o lookuptriangular.o  lookupseno.o lookupsquare.o freertos/event_groups.o freertos/heap_3.o freertos/hooks.o  freertos/list.o freertos/port.o freertos/queue.o freertos/stream_buffer.o freertos/tasks.o freertos/timers.o
	avr-objcopy -j .text -j .data -O ihex $(FILENAME).elf $(FILENAME).hex
	avr-size -C --mcu=$(DEVICE) $(FILENAME).elf

upload:
	avrdude -v -p $(DEVICE) -c $(PROGRAMMER) -P $(PORT) -b $(BAUD) -U flash:w:$(FILENAME).hex:i

clean:
	rm *.o
	rm freertos/*.o
	rm *.elf
	rm *.hex
