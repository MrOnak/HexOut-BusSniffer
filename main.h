#define BUS_WIDTH_BYTE 4 // bus width in byte

#define REGISTER_SHIFTREG	   DDRB
#define PORT_SHIFTREG		     PORTB
#define PORT_IN_SHIFTREG	   PINB
#define PIN_SERIAL_IN        PB0          // arduino pin 8
#define PIN_SERIAL_CLOCK     PB1          // arduino pin 9
#define PIN_SERIAL_SHIFTLOAD PB2          // arduino pin 10

#define REGISTER_BYTEPOINTER	DDRC
#define PORT_BYTEPOINTER		  PORTC
#define PORT_IN_BYTEPOINTER	  PINC
#define PIN_BYTEPOINTER       PC0         // arduino pin A0

/**
 * checks for the button press that moves the bytePointer
 */
void checkBytePointer(void);

/**
 * performs the serial-in from the 74*165 registers and returns the long containing the data
 *
 * @return uint32_t
 */
uint32_t readShiftRegisters(void);

/**
 * displays all data on the bus
 */
void displayBus(void);

/**
 * displays the selected byte in binary
 */
void displayBinary(void);