# HexOut-BusSniffer

Little gadget that offers 32-bit wide parallel read (i.e. of a bus) and displays the data on a 16x2 LCD. Uses the ATMega328p and some 74*165 shift registers.

I've built this as debugger for all sorts of situations where parallel data must be investigated. The idea is heavily inspired by Quinn Dunki and her
[HexOut](http://quinndunki.com/blondihacks/?p=610) circuit which basically does the same but for 16 bit and with 7-segment LEDs. I had a 16x2 LCD lying
around and it simplifies the circuit considerably so that's what I went with. 

##  Specification
- All parallel inputs are protected with 100k resistors.
- The circuit draws around 37mA of current with the backlight on the LCD on. 
- the compiled code is less than 2k in size and should fit on an Attiny2313.
- I chose the Atmega328p microprocessor since my ATtiny2313 development board wasn't working but frankly, this is overkill.
- **not sure** whether the Attiny2313 actually has enough pins to drive the circuit but I suspect it does.
- the board provides 6-pin ISP access, a tactile reset button and a power LED. a second pushbutton is used by the software
- 2x serial register communication (clock, data, shift/load) access pins are provided on the perfboard
- the perfboard provides multiple access points for power
- there is **no protection** against reverse voltage or high voltage.
- floating inputs are not disabled on the display

## Basics of operation

The data is read in via four daisy-chained 74*165 parallel-in/serial-out shift registers. 
If the data differs from the one read previously, the LCD screen is updated.

Data is read as fast as the microprocessor can handle. No external clock signal is recognized to sync the data read with the clock 
on the external bus. With fast operations on the bus the display might intermittently show invalid data, depending on how the bus works.

### LCD top row 

The top row on the LCD displays the 32 bit in four groups of one byte each. The display is in hexadecimal.
An example output on the top row could be this:

    "   FF 02 01>AD  "

"FF" indicates the MSB (left of the perfboard connector), "AD" are the LSB (right on the perfboard connector). The ">" character is the 
byte select indicator, more below.

### LCD bottom row

The bottom row displays the selected byte as binary. An input line that is high produces a "1", a low input line is shown as "0".

In the example above, the ">" indicates that the LSB "AD" is to be displayed in the bottom row in binary. The bottom row will read

    "    10101101    "

The perfboard includes a pushbutton (somwhat debounced with a capacitor) which alternates the selected byte for the binary
display in a round robin fashion. 

### What this circuit is not

It is not a data logger. Data is presented as-is and will be overwritten with the next read cycle. The pin-outs on the
perfboard allow for easy extension, so a data logger can possibly connected relatively easily.
