/*************************************************************************
Title:    game_console Template
Initial Author:   David Jahshan
Extended by : Pim 757920, 2018
Software: AVR-GCC 
Hardware: ATMEGA16 @ 8Mhz 

DESCRIPTION:
	Main and functions for Game Console to complete an etch-sketch. 

*************************************************************************/


#include "game_console.h" 
#include <util/delay.h>

/* global variables */ 
byte frame_buffer[MAX_COLUMNS][MAX_PAGES];
byte pixel, page; 
byte column = 51; 
byte row = 32; 


byte LCD_send_text(byte tx_byte); 
byte LCD_command_tx(byte tx_byte);
byte LCD_data_tx(byte tx_byte); 
byte select_page (byte page);
byte select_column (byte column);
void pixel_draw(void);
void clear_screen(void);
void SPI_MasterInit(void);
byte LCD_initialise(void);
void ADC_read(void);
void initialise_routine(void);

void initialise_FRAM(void);
byte send_data_FRAM(byte tx_byte);
void enable_write_FRAM(void);
void disable_write_FRAM(void);
void write_FRAM(byte col, byte page, byte data);
void read_FRAM(byte size);


 
int main(void) {

	initialise_routine();
	pixel_draw();
	

	while (TRUE)//Master loop always true so always loop
	{
		//BAT_LOW_LED(OFF);
		ADC_read();	  
	 	
	}

}



/* -------------------------------- 

COMMUNICATING TO THE LCD COMMANDS 

------------------------------------*/

byte LCD_send_tx(byte tx_byte) {
	/* Start transmission */
	SPDR = tx_byte;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return(TRUE); 
} 


byte LCD_command_tx(byte tx_byte) {

	LCD_CS_SET(LOW); //turns on low because active low
	LCD_CD_COMMAND;
	LCD_send_tx(tx_byte); 
	LCD_CS_SET(HIGH);	 

	return(TRUE);

}

// Sends data to the LCD 
byte LCD_data_tx(byte tx_byte) {

	LCD_CS_SET(LOW);
	LCD_CD_DATA; 

	LCD_send_tx(tx_byte);

	LCD_CS_SET(HIGH);
	
	return(TRUE);	
}


/* ---------------------------------

LCD PIXEL SELECTION

------------------------------------*/


byte select_page (byte page) {

	if (page > MAX_PAGES){
			page = MAX_PAGES-1;
		}
	byte page_cmd_address;
	page_cmd_address =(CMD_PAGE | page);
	LCD_command_tx(page_cmd_address);
	
	return(TRUE);
}
 


byte select_column (byte column) {

	if (column > MAX_COLUMNS) { 
		column = MAX_COLUMNS-1; 


	}	
		byte page_cmd_address_MSB;
		byte page_cmd_address_LSB;
		page_cmd_address_LSB =(CMD_COL_LSB | (column & 0x0F));
		page_cmd_address_MSB =(CMD_COL_MSB | (column >> 4));
		LCD_command_tx(page_cmd_address_LSB);
		LCD_command_tx(page_cmd_address_MSB);
		return(TRUE);
}

void pixel_draw(void) {

	page = row/8; 
	pixel = row%8; 
	select_page(page); 
	select_column(column);
	pixel = (_BV(pixel)| frame_buffer[column][page]);
	frame_buffer[column][page] = pixel; 
	LCD_data_tx(pixel);
	_delay_ms(125); 

}




/* --------------------------------------------

Initialising Routines

-----------------------------------------------*/

void clear_screen(void) { 

	int i, j; 

	for(i=0; i<=MAX_PAGES; i++){ 

		
		for(j=0; j<=MAX_COLUMNS; j++){ 
			select_page(i);
			select_column(j);
			LCD_data_tx(0x00);
			frame_buffer[j][i] = OFF;
		}

	}
}




void SPI_MasterInit(void) {
	
	DDRB = _BV(PB5)|_BV(PB6); 	

	SCK_DIR(OUT);
    MOSI_DIR(OUT);
    MISO_DIR(IN);
	
	SPCR = _BV(SPE)|_BV(MSTR)|_BV(SPR0);
	
} 


 byte LCD_initialise(void) {
	
	LCD_CS_DIR(OUT);
	LCD_CS_SET(LOW); // Chip Select of LCD set to low
	
	LCD_CD_DIR(OUT);
	LCD_CD_COMMAND; // LCD put into command (LOW) mode
	LCD_RESET_DIR(OUT); 
	LCD_RESET_SET(LOW);
	_delay_ms(10);
	LCD_RESET_SET(HIGH);
	_delay_ms(5);
	
	LCD_command_tx(0x40);//Display start line 0 
	LCD_command_tx(0xA1);//SEG reverse 

	LCD_command_tx(0xC0);//Normal COM0~COM63	
	LCD_command_tx(0xA4);//Disable -> Set All Pixel to ON
	LCD_command_tx(0xA6);//Display inverse off
	_delay_ms(120);	
	LCD_command_tx(0xA2);//Set LCD Bias Ratio A2/A3
	LCD_command_tx(0x2F);//Set Power Control 28...2F
	LCD_command_tx(0x27);//Set VLCD Resistor Ratio 20...27
	LCD_command_tx(0x81);//Set Electronic Volume
	LCD_command_tx(0x10);//Set Electronic Volume 00...3F
	LCD_command_tx(0xFA);//Set Adv. Program Control
	LCD_command_tx(0x90);//Set Adv. Program Control x00100yz yz column wrap
	LCD_command_tx(0xAF);//Display on

	return(TRUE);
}  



/* --------------------------------------

INTERRUPT ROUTINE

-----------------------------------------*/

ISR(INT1_vect) {


	while(INTERRUPT) {

	if(BRIGHT_BUTTON) {
			OCR2 += 64;   
	} if(UP_BUTTON){ 
		row--;		
					
	} if(DOWN_BUTTON) {
		row++; 				
	} if(LEFT_BUTTON) {
		column--; 	

	} if(RIGHT_BUTTON) {
		column++;
	} if(OK_BUTTON) {

	BAT_LOW_LED(ON);

	}
	pixel_draw();  


	}


}



/* -------------------------------

Initialising Everything

----------------------------------*/

void initialise_routine(void) {

	BAT_LOW_LED(OFF); //Make sure it is off before changing direction
	BAT_LOW_LED_DIR(OUT); //Set BATTERY LED I/Os as outputs.
	

	UP_BUTTON_DIR(IN); //Set UP_BUTTON I/Os as input
	BRIGHT_BUTTON_DIR(IN);
	LEFT_BUTTON_DIR(IN);
	DOWN_BUTTON_DIR(IN);
	RIGHT_BUTTON_DIR(IN);
	OK_BUTTON_DIR(IN);
	SPARE_BUTTON_DIR(IN);
	

	OK_BUTTON_SET; 
	SPARE_BUTTON_SET;

	
	UP_BUTTON_SET;
	BRIGHT_BUTTON_SET; 	
	DOWN_BUTTON_SET;
	RIGHT_BUTTON_SET;
	LEFT_BUTTON_SET;


	INTERRUPT_SETUP;

	//SET UP RAM

	
	//PWM INITIALISATION
	TCCR2|=(1<<WGM20)|(1<<WGM21)|(1<<COM21)|(1<<CS20);
	MICRO_PWM_DIR(OUT);

	ADC_DIR(IN);
	ADC_SETUP;		


	LCD_SS_DIR(OUT); 
	LCD_SS_SET(HIGH);
	SPI_MasterInit();


	SCK_SET(ON);	

	/* Initialising stuff */	
	
	LCD_initialise();
	clear_screen();
	
}

/* -----------------------------------

INITIALISE THE FRAM 
--------------------------------------*/

void initialise_FRAM(void) {
	MISO_SET(IN);
	CS_FRAM_DIR(OUT); 
	WRITE_FRAM_DIR(OUT);
	HOLD_FRAM_DIR(OUT);
	HOLD_FRAM_SET(ON);
	WRITE_FRAM_SET(OFF);

}

byte send_data_FRAM(byte tx_byte){

	/*Write tx_byte to the SPI data register */
	SPDR = tx_byte;
	/*Busywait while the transfer isn't done yet */ 
	while (!(SPSR & (1 << SPIF)));
	
	return(SPDR);

}

void enable_write_FRAM(void) {
	cli(); // clear interrupt flags while writing  
	CS_FRAM_SET(LOW); // active low 
	send_data_FRAM(RAM_WREN); //send the bits required to enable write
	CS_FRAM_SET(HIGH); 

}

void disable_write_FRAM(void) {
	CS_FRAM_SET(LOW); 
	send_data_FRAM(RAM_WRDI); 
	CS_FRAM_SET(HIGH); 
	sei(); // activate interrupts again 
}

void write_FRAM(byte col, byte page, byte data) {	
	enable_write_FRAM(); 
	
	CS_FRAM_SET(LOW); 
	send_data_FRAM(RAM_WRITE); 
	
	send_data_FRAM(col); 
	send_data_FRAM(page); 
	send_data_FRAM(data);
	

	CS_FRAM_SET(HIGH);
	disable_write_FRAM(); 

} 



/* -----------------------------------

reading the ADC

--------------------------------------*/

void ADC_read(void){ 	

	ADMUX = _BV(MUX1)|_BV(MUX0); //selects ADC3

	ADCSRA |= _BV(ADSC); // starts the conversion 

	while(ADCSRA & (_BV(ADSC))); //run until the conversion is complete 

	
	if (ADC < LOW_BAT) {

		BAT_LOW_LED(ON);

	}

}

