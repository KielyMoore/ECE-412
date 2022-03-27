  // Lab3P1.s
 //
 // 
 // Author : Eugene Rockey
 // Copyright 2022, All Rights Reserved


.section ".data"					//student comment here
.equ	DDRB,0x04					//(0100) PB2 set to output, rest set to inputs
.equ	DDRD,0x0A					//(1010) PortB bit 1 and 3 set to output, rest to input
.equ	PORTB,0x05					//(0000 0101) to Port B ****what is address offset? 
.equ	PORTD,0x0B					//(0000 1011) to Port D 
.equ	U2X0,1						//Double USART transmission speed
.equ	UBRR0L,0xC4					//(1100 0100) to Baud Rate 0 Low Reg (4MSB)
.equ	UBRR0H,0xC5					//(1100 0101) to Baud rate 0 high reg (8LSB)
.equ	UCSR0A,0xC0					//(1100 0000) USART Control & Status Reg 0 A
									//RXC0 flag bit set (?), TXC0 flag bit set
.equ	UCSR0B,0xC1					//USART Control & Status Reg 0 B
.equ	UCSR0C,0xC2					//USART Control & Status Reg 0 C
.equ	UDR0,0xC6					//USART I/O Data Reg 0 
.equ	RXC0,0x07					//
.equ	UDRE0,0x05					//
.equ	ADCSRA,0x7A					//ADC Control & Status Reg A
.equ	ADMUX,0x7C					//ADC Multiplexer Selection Reg
.equ	ADCSRB,0x7B					//ADC Control & Status Reg B
.equ	DIDR0,0x7E					//Digital Input Disable Reg 0
.equ	DIDR1,0x7F					//Digital Input Disable Reg 1
.equ	ADSC,6						//ADC Start Conversion
.equ	ADIF,4						//ADC Interrupt Flag
.equ	ADCL,0x78					//ADC Data Reg Low (ADLAR=0)
.equ	ADCH,0x79					//ADC Data Reg High (ADLAR=0)
.equ	EECR,0x1F					//EEPROM Control Reg
.equ	EEDR,0x20					//EEPROM Data Reg
.equ	EEARL,0x21					//EEPROM Address Reg Low
.equ	EEARH,0x22					//EEPROM Address Reg High
.equ	EERE,0						//EEPROM Read Enable
.equ	EEPE,1						//EEPROM Write Enable
.equ	EEMPE,2						//EEPROM Master Write Enable
.equ	EERIE,3						//EEPROM Ready Interrupt Enable

.global HADC				//Declare HADC as global variable
.global LADC				//Declare LADC as global variable
.global ASCII				//Declare ASCII as global variable
.global DATA				//Declare DATA as global variable

.set	temp,0				//set temp value to 0

.section ".text"			//Initialize .text section
.global Mega328P_Init
Mega328P_Init:				//Initialize Mega, 
		
		//Direction of data for Port B and D
		ldi	r16,0x07		;PB0(R*W),PB1(RS),PB2(E) as fixed outputs
		out	DDRB,r16		//0111 sets bits 0:2 of port B as outputs
		ldi	r16,0			//Load 0 into reg 16
		out	PORTB,r16		//Value of r16 into PORTB, RESET
		
		out	U2X0,r16		;initialize UART, 8bits, no parity, 1 stop, 9600
		
		//Set Baud rate
		ldi	r17,0x0			//Load 0 into r17
		ldi	r16,0x67		//Load r16 with 0110 0111
		sts	UBRR0H,r17		//Set Baud Rate 0 High to 0000
		sts	UBRR0L,r16		//Set Baud Rate 0 High to 0110 0111
	
		//Enable Reciever/Transmitter, 
		ldi	r16,24			//Load decimal 24 (0001 1000) into reg 16
		sts	UCSR0B,r16		//Enables Reciever and Transmitter RXENn/TXENn
	
		//Set mode, parity, stop bit and bit rate
		ldi	r16,6			//Load decimal 6 (0110) into r16
		sts	UCSR0C,r16		//USART Mode = Asynch, Parity Disabled, 1 stop bit, bit rate=8-bits
	
		//Init ADC
		ldi r16,0x87		//initialize ADC, loads r16 with (1000 0111)
		sts	ADCSRA,r16		//Enable ADC, Set division factor to 128 between clock freq&input clock
		

		//*****ASK ABOUT HOW THIS IS WORKING******

		//Multiplexer Selection Register
		//REFS = 01, AVcc w/ External Capacitor at AREF ****
		//ADLAR, ADC Left Adjust Result, result is right adjusted
		//MUX, Anal Chann Select, 0000, ADC0 (remember that Temp Sensor = 1000?)
		ldi r16,0x40		//01000000 into r16
		sts ADMUX,r16		//Ref Selection, AVcc w/External capacitor at AREF pin, ADC0 connected=PC[0]
		
		//ADC Control and Status Reg B
		//Analog Comparator Multiplexer Enable (bit 6) set to low, 
		//AIN1 is applied to neg input of analog comparator (see Analog Comparator Multiplexed Input)
		//Bits 2:0, ADTS, ADC Auto Trigg Source, 000, Free running mode
		ldi r16,0			//load 0000 into r16
		sts ADCSRB,r16		//Free running mode
		
		//Digital Input Disable Registers 1 & 0
		
		//DIDR0 Digital Input Disable Register 0
		//Bits written to 1, digital input buffer is disabled. 
		//Pin will always read zero if set
		;Is this saying we don't need ADC6D
		ldi r16,0xFE		//0100 0000
		sts DIDR0,r16		//Disable ADC6D
		
		//DIDR1 Digit Input Disable Register 1
		//AIN1D, AIN0D, AIN Digital Input Disable
		//When written high, digital input buffer on AIN1/0 disabled
		ldi r16,0xFF		//1111 1111
		sts DIDR1,r16		//Disable AIN1/0 pins
		ret					//student comment here
	
.global LCD_Write_Command
LCD_Write_Command:
	call	UART_Off		//Call UART_Off
	ldi		r16,0xFF		;PD0 - PD7 as outputs
	out		DDRD,r16		//1111 1111, Enable all D as outputs
	lds		r16,DATA		//load char Data into PORTD
	out		PORTD,r16		//Instruction code
	ldi		r16,4			//load 0100 to r16
	out		PORTB,r16		//Set PB2 to 1, Enable E, Write, Instruction Input
	call	LCD_Delay		//Call LCD_Delay
	ldi		r16,0			//0000 to r16
	out		PORTB,r16		//Set PORTB pins to 0, Disable, Write, Instruction Input
	call	LCD_Delay		//Call LCD_Delay
	call	UART_On			//Call UART_On
	ret						//student comment here

LCD_Delay:
	ldi		r16,0xFA		//load 1111 1010 to r16
D0:	ldi		r17,0xFF		//load 1111 1111 to r17
D1:	dec		r17				//Decrement r17 
	brne	D1				//Branch to D1 if r17!=0
	dec		r16				//Decrement r16
	brne	D0				//Branch to D0 if r16!=0
	ret						//Return

.global LCD_Banner
LCD_Banner:
	call	LCD_Write_Command	//LCD command to shift display right
	call	LCD_Delay			//call to LCD Delay
	call	LCD_Delay			//call to LCD Delay
	lds	r16,UCSR0A				//
	sbrs	r16,RXC0			//student comment here
	rjmp	LCD_Banner			//student comment here
	ret							//student comment here


.global LCD_Write_Data
LCD_Write_Data:
	call	UART_Off		//Turn off UART_Off
	ldi		r16,0xFF		//1111 1111 to r16
	out		DDRD,r16		//all of PortD to output
	lds		r16,DATA		//DATA = *str++ to r16
	out		PORTD,r16		//Data pushed out thru PortD
	ldi		r16,6			//0110 to r16
	out		PORTB,r16		//Enable, Write, Data Input
	call	LCD_Delay		//Jump to LCD delay
	ldi		r16,0			//0000 0000 to r16
	out		PORTB,r16		//Clear display
	call	LCD_Delay		//Call LCD_Delay
	call	UART_On			//Call UART_On
	ret						//student comment here

.global LCD_Read_Data
LCD_Read_Data:
	call	UART_Off		//Call UART_Off, turn off Rec/Trans
	ldi		r16,0x00		//load 0 into r16
	out		DDRD,r16		//Set PortD to inputs
	out		PORTB,4			//Enable display (0100) (E=H, Write, Instruction)
	in		r16,PORTD		//PortD to r16
	sts		DATA,r16		//Save portD values to DATA
	out		PORTB,0			//Zero out port B
	call	UART_On			//Call UART_On
	ret						//student comment here

.global UART_On
UART_On:
	ldi		r16,2				//Load 0010 to r16
	out		DDRD,r16			//Sets direction of pin 1 to output
	ldi		r16,24				//loads 0001 1000 to r16
	sts		UCSR0B,r16			//0001 1000 to USCR0B, enable Receiver/Transmitter
	ret							//Return 

.global UART_Off
UART_Off:
	ldi	r16,0					//0000 to r16
	sts UCSR0B,r16				//Turn off Receiver&Transmitter
	ret							//student comment here

.global UART_Clear
UART_Clear:
	lds		r16,UCSR0A			//Load value of UCSR0A to r16
	sbrs	r16,RXC0			//Skip if Bit in Reg is Set
	ret							//return
	lds		r16,UDR0			//Load UDR0 to r16
	rjmp	UART_Clear			//student comment here

.global UART_Get
UART_Get:
	lds r16,UCSR0A //load r16 with USART Status Reg 0A
	sbrs r16,RXC0 //skip next line if USART Receive Complete is set
	rjmp UART_Get //jump to UART_Get
	lds r16,UDR0 //load r16 with USART Data Reg 0
	sts ASCII,r16 //Set ASCII with r16
	ret //return to above function

.global UART_Put
UART_Put:
	lds r17,UCSR0A //load r17 with USART Status Reg 0A
	sbrs r17,UDRE0 //skip next line if UDR is ready to receive new data
	rjmp UART_Put //jump back to top
	lds r16,ASCII //load r16 with ASCII
	sts UDR0,r16 //Set USART Data Reg 0 with r16
	ret //return to above function

.global ADC_Get
ADC_Get:
		ldi r16,0xC7	//Load r16 with value 1100 0111
		sts ADCSRA,r16	//ADC Enable, Start Convers, 128 division factor
A2V1:	lds r16,ADCSRA	//Load r16 with ADC Status Reg A
		sbrc r16,ADSC	//branch if ADC Start Conv bit clear ****it's always going to be cleared....
		rjmp A2V1		//Returns to A2V1
		//Load values of ADC Low into r16
		lds r16,ADCL	//Load r16 with ADC High bit
		sts LADC,r16	//Set LADC with r16
		lds r16,ADCH	//Load r16 with ADC High bit
		sts HADC,r16	//Set HADC with r16
		ret				//return to above function

.global EEPROM_Write
EEPROM_Write:      
		sbic    EECR,EEPE
		rjmp    EEPROM_Write		; Wait for completion of previous write
		ldi		r18,0x00			; Set up address (r18:r17) in address register
		ldi		r17,0x05 
		ldi		r16,'F'				; Set up data in r16    
		out     EEARH, r18      
		out     EEARL, r17			      
		out     EEDR,r16			; Write data (r16) to Data Register  
		sbi     EECR,EEMPE			; Write logical one to EEMPE
		sbi     EECR,EEPE			; Start eeprom write by setting EEPE
		ret 

.global EEPROM_Read
EEPROM_Read:					    
		sbic    EECR,EEPE    
		rjmp    EEPROM_Read		; Wait for completion of previous write
		ldi		r18,0x00		; Set up address (r18:r17) in EEPROM address register
		ldi		r17,0x05
		ldi		r16,0x00   
		out     EEARH, r18   
		out     EEARL, r17		   
		sbi     EECR,EERE		; Start eeprom read by writing EERE
		in      r16,EEDR		; Read data from Data Register
		sts		ASCII,r16  
		ret


		.end

	
