#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "main.h"
#include "lcd/lcd.h"

#define BUS_WIDTH BUS_WIDTH_BYTE * 8   // bus width in bit
#define PULSE_WIDTH 5

#define LOW 0
#define HIGH 1

/**
 * holds 32-bit data in a convenient format
 */
typedef union {
  uint8_t byteVal[4];
  uint32_t longVal;
} u_busdata_t;

/**
 * current busData
 */
u_busdata_t busData;

/**
 * used to track changes and trigger LCD updates
 */
u_busdata_t oldBusData;

/**
 * determines which of the 4 bytes will be displayed as binary.
 * changes through the pushbutton
 */
uint8_t bytePointer;

/**
 * since we're not using interrupts we're tracking whether the
 * button goes up or down with this
 */
uint8_t bytePointerState;

/**
 * flag whether or not the LCD needs to be updated.
 *
 * two scenarios set this to true: new bus data and a changed bytePointer
 */
uint8_t updateDisplay;

/**
 * main routine
 */
int main(void) {
  updateDisplay = LOW;
  
  // set all pins as output
  REGISTER_SHIFTREG = 0xFF; 
  // configure output of 74HTC165N shift register as input
  REGISTER_SHIFTREG &= ~(1 << PIN_SERIAL_IN);
  // clock and shift/load pins as output
  REGISTER_SHIFTREG |= (1 << PIN_SERIAL_CLOCK);
  REGISTER_SHIFTREG |= (1 << PIN_SERIAL_SHIFTLOAD);
  
  // set all pins as output
  REGISTER_BYTEPOINTER = 0xFF;
  // pushbutton is an input
  REGISTER_BYTEPOINTER &= (~1 << PIN_BYTEPOINTER);

  // set shift/load high to prevent reading of data
  PORT_SHIFTREG |= (1 << PIN_SERIAL_SHIFTLOAD);
  // set clock pin low
  PORT_SHIFTREG &= ~(1 << PIN_SERIAL_CLOCK);
  
  busData.longVal    = 0x0000;
  oldBusData.longVal = 0x0000;
  bytePointer        = 0;
  bytePointerState   = LOW;
    
  // display welcome screen for 2 seconds to confirm everything is working
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_puts("bus display v1.0");
  _delay_ms(2000);
  lcd_clrscr();
  
  while (1) {
    checkBytePointer();
    busData.longVal = readShiftRegisters();
    
    if (busData.longVal != oldBusData.longVal) {
      updateDisplay = HIGH;
    }
    
    if (updateDisplay == HIGH) {
      displayBus();
      oldBusData = busData;
    }

    displayBinary();
  }
  
  return 0;
}


/**
 * reads in all data from the 74*165 shift registers and returns the data
 *
 * @return uint32_t
 */
uint32_t readShiftRegisters(void) {
  uint32_t bitVal;
  uint32_t busVal = 0;
  uint8_t i;

  // Trigger a parallel load to latch the state of the data lines
  PORT_SHIFTREG &= ~(1 << PIN_SERIAL_SHIFTLOAD); 
  _delay_us(PULSE_WIDTH);
  PORT_SHIFTREG |= (1 << PIN_SERIAL_SHIFTLOAD);

  // Loop to read each bit value from the serial out line of the shift register
	// we forge the keyboard report directly, to safe some memory
  for (i = 0; i < BUS_WIDTH; i++) {
    bitVal = PORT_IN_SHIFTREG & (1 << PIN_SERIAL_IN);
    busVal |= (bitVal << ((BUS_WIDTH - 1) - i));

    // Pulse the clock (rising edge shifts th next bit).
    PORT_SHIFTREG |= (1 << PIN_SERIAL_CLOCK);
    _delay_us(PULSE_WIDTH);
    PORT_SHIFTREG &= ~(1 << PIN_SERIAL_CLOCK);
  }
  		
	return busVal;	
}


/**
 * checks for the button press that moves the bytePointer
 */
void checkBytePointer(void) {
  uint8_t pinVal = PORT_IN_BYTEPOINTER & (1 << PIN_BYTEPOINTER);
  
  if (bytePointerState == 1 && pinVal == 0) {
    bytePointer = (bytePointer + 1) % BUS_WIDTH_BYTE;
    updateDisplay = HIGH;
  }

  bytePointerState = pinVal;
}

/**
 * displays all data on the bus
 */
void displayBus(void) {
  // converts pure numeric data to ASCII of 0-9A-F:
  // 0x00 will be 0x48 (ASCII '0'), 0x01 will be 0x49 (ASCII '1'), etc.
  uint8_t displayConversion[16] = {
   48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 65, 66, 67, 68, 69, 70
  };  
  uint8_t i;
  
  lcd_gotoxy(2, 0); 
  
  for (i = 0; i < BUS_WIDTH_BYTE; i++) {
    lcd_putc(' ');
    lcd_putc(displayConversion[0x0F & (busData.byteVal[i] >> 4)]);
    lcd_putc(displayConversion[0x0F & (busData.byteVal[i])]);
  }
}

/**
 * displays the selected byte in binary
 */
void displayBinary(void) {
  uint8_t i;
  
  // display byte pointer
  lcd_gotoxy(11 - bytePointer * 3, 0);
  lcd_putc('>');

  // display the selected byte as binary
  lcd_gotoxy(4, 1);
  
  for (i = 0; i < 8; i++) {
    if ((busData.byteVal[BUS_WIDTH_BYTE - (bytePointer+1)] & (1 << (7-i)))) {
      lcd_putc('1');
    } else {
      lcd_putc('0');
    }
  }
}