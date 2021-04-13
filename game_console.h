/*************************************************************************
Title:    Game Console Template Header File
Inital Author:   David Jahshan
Extended by : Pim 757920
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Macros for Game Console

*************************************************************************/

#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define byte unsigned char 

/*ON OFF*/
#define ON 0xFF
#define OFF 0x00
#define HIGH 0xFF
#define LOW 0x00
#define IN 0x00
#define OUT 0xFF
#define ALL 0xFF
#define TRUE 1
#define FALSE 0
#define FORWARD 0x00
#define BACK 0xFF
#define MAX_PAGES 7 //pages 0 to 7
#define MAX_COLUMNS 102
#define CMD_COL_LSB 0x00
#define CMD_COL_MSB 0x10
#define CMD_PAGE 0xB0
#define MIDDLE_PAGE 4
#define MIDDLE_COLUMN 50
#define MAX_ROWS 64
#define LOW_BAT 1.5




#define RAM_WRSR 		0b00000001
#define RAM_WREN 		0b00000110
#define RAM_WRDI		0b00000100
#define RAM_READ		0b00000011
#define RAM_WRITE	  	0b00000010
#define RAM_SR_INIT	    0b10000000



/*SET and GET MACRO*/
#define SET(PORT,MASK,VALUE) PORT = ((MASK & VALUE) | (PORT & ~MASK))
#define GET(PORT,MASK) PORT & MASK

//Devices Direction MACROs
//#define MULTI_LINES_DIR(DIR) SET(DDR?,(_BV(P?#)|_BV(P?#)|_BV(P?#)),DIR)
#define BAT_LOW_LED_DIR(DIR) SET(DDRA,_BV(PA4),DIR)


#define MOSI_DIR(DIR) SET(DDRB, _BV(PB5), DIR) 
#define MISO_DIR(DIR) SET(DDRB, _BV(PB6), DIR)
#define SCK_DIR(DIR) SET(DDRB, _BV(PB7), DIR)
#define LCD_DIR(DIR) SET(DDRA, _BV(PA0), DIR)
 
 //LCD DIRECTION MACROs 
#define LCD_CD_DIR(DIR) SET(DDRA, _BV(PA0), DIR)
#define LCD_CS_DIR(DIR) SET(DDRA, _BV(PA2), DIR)
#define LCD_RESET_DIR(DIR) SET(DDRA, _BV(PA1), DIR)
#define LCD_SS_DIR(DIR) SET(DDRB, _BV(PB4), DIR)
#define MICRO_PWM_DIR(DIR) SET(DDRD, _BV(PD7), DIR) 


//buttons
#define UP_BUTTON_DIR(DIR) SET(DDRD,_BV(PD5),DIR)
#define LEFT_BUTTON_DIR(DIR) SET(DDRD,_BV(PD4),DIR)
#define RIGHT_BUTTON_DIR(DIR) SET(DDRD, _BV(PD6), DIR) 
#define DOWN_BUTTON_DIR(DIR) SET(DDRD, _BV(PD1), DIR)
#define OK_BUTTON_DIR(DIR) SET(DDRC, _BV(PC0), DIR)
#define SPARE_BUTTON_DIR(DIR) SET(DDRC, _BV(PC6), DIR)



#define BRIGHT_BUTTON_DIR(DIR) SET(DDRC, _BV(PC1), DIR)


//RAM DIRECTIONS 
#define CS_FRAM_DIR(DIR) SET(DDRB, _BV(PB1), DIR) 
#define HOLD_FRAM_DIR(DIR) SET(DDRB, _BV(PB2), DIR) 
#define WRITE_FRAM_DIR(DIR) SET(DDRB, _BV(PD2), DIR) 

//RAM outputs 
#define CS_FRAM_SET(STATE) SET(PORTB, _BV(PB1), STATE)
#define HOLD_FRAM_SET(STATE) SET(PORTB, _BV(PB2), STATE)
#define WRITE_FRAM_SET(STATE) SET(PORTD, _BV(PD2), STATE)  

//Devices Outputs

#define BAT_LOW_LED(STATE) SET(PORTA,_BV(PA4),~STATE)
#define MOSI_SET(STATE) SET(PORTB, _BV(PB5), STATE) 
#define MISO_SET(STATE) SET(PORTB, _BV(PB6), STATE)



//LCD OUTPUTS 
#define LCD_CD_DATA SET(PORTA, _BV(PA0), HIGH)
#define LCD_CD_COMMAND SET(PORTA, _BV(PA0), LOW)
#define LCD_CS_SET(STATE) SET(PORTA, _BV(PA2), STATE)

#define LCD_RESET_SET(STATE) SET(PORTA, _BV(PA1), STATE)
#define LCD_SS_SET(STATE) SET(PORTB, _BV(PB4), STATE)


#define MICRO_PWM_SET(STATE) SET(PORTD, _BV(PD7), STATE)
#define SCK_SET(STATE) SET(PORTB, _BV(PB7), STATE)

//activate pull up resistors
#define UP_BUTTON_SET SET(PORTD, _BV(PD5), ON); 
#define BRIGHT_BUTTON_SET SET(PORTC, _BV(PC1), ON); 	
#define DOWN_BUTTON_SET SET(PORTD, _BV(PD1), ON);
#define RIGHT_BUTTON_SET SET(PORTD, _BV(PD6),ON); 
#define LEFT_BUTTON_SET SET(PORTD, _BV(PD4), ON);
#define OK_BUTTON_SET SET(PORTC, _BV(PC0), ON); 
#define SPARE_BUTTON_SET SET(PORTC, _BV(PC6), ON); 

//Devices Inputs buttons (all active low add ~) 
#define UP_BUTTON GET(~PIND,_BV(PD5))
#define BRIGHT_BUTTON GET(~PINC, _BV(PC1))
#define LEFT_BUTTON  GET(~PIND, _BV(PD4))
#define DOWN_BUTTON GET(~PIND, _BV(PD1))
#define RIGHT_BUTTON GET(~PIND, _BV(PD6))
#define OK_BUTTON GET(~PINC, _BV(PC0))
#define SPARE_BUTTON GET(~PINC, _BV(PC6))


//Interrupts
//trigger on any chance in logic value
#define INTERRUPT_SETUP SET(GICR,_BV(INT1),ON);SET(MCUCR,_BV(ISC10),ON);\
						sei()
#define INTERRUPT GET(PIND, _BV(PD3)) 


//DEFINE ADCs
// set up reference voltage with cap, set status register enable, 
#define ADC_DIR(DIR) SET(DDRA, _BV(PA3), DIR)
#define ADC_INPUT GET(~PINA, _BV(PA3))
#define ADC_SETUP SET(ADMUX, _BV(REFS0), ON); SET(ADCSRA, (_BV(ADEN)|_BV(ADPS1)|_BV(ADPS0)), ON)
