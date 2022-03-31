
 // Lab3P1.c
 //
 // 
 // Author : Eugene Rockey
 // 
 
 //no includes, no ASF, no libraries
#include <math.h>
#include <stdlib.h>
 
 const char MS1[] = "\r\nECE-412 ATMega328PB Tiny OS";
 const char MS2[] = "\r\nby Eugene Rockey Copyright 2022, All Rights Reserved";
 const char MS3[] = "\r\nMenu: (L)CD, (A)DC, (E)EPROM\r\n";
 const char MS4[] = "\r\nReady: ";
 const char MS5[] = "\r\nInvalid Command Try Again...";
 const char MS6[] = "Volts\r";
 
 

void LCD_Init(void);			//external Assembly functions
void UART_Init(void);
void UART_Clear(void);
void UART_Get(void);
void UART_Put(void);
void LCD_Write_Data(void);
void LCD_Write_Command(void);
void LCD_Read_Data(void);
void LCD_Banner(void);
void Mega328P_Init(void);
void ADC_Get(void);
void EEPROM_Read(void);
void EEPROM_Write(void);
void ReadTemp(void);
void Get_Input(void);

unsigned char ASCII;			//shared I/O variable with Assembly
unsigned char DATA;				//shared internal variable with Assembly
char HADC;						//shared ADC variable with Assembly
char LADC;						//shared ADC variable with Assembly

char Tempf[5];					//string buffer for ADC output
int Acc;						//Accumulator for ADC use
int r;
int tempk, tempc, tempf;

void UART_Puts(const char *str)	//Display a string in the PC Terminal Program
{
	while (*str)
	{
		ASCII = *str++;
		UART_Put();
	}
}

void LCD_Puts(const char *str)	//Display a string on the LCD Module
{
	while (*str)
	{
		DATA = *str++;
		LCD_Write_Data();
	}
}


void Banner(void)				//Display Tiny OS Banner on Terminal
{
	UART_Puts(MS1);
	UART_Puts(MS2);
	UART_Puts(MS4);
}

void HELP(void)						//Display available Tiny OS Commands on Terminal
{
	UART_Puts(MS3);
}

void LCD(void)						//Lite LCD demo
{
	DATA = 0x34;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x08;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x02;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x06;					//Student Comment Here
	LCD_Write_Command();
	DATA = 0x0f;					//Student Comment Here
	LCD_Write_Command();
	LCD_Puts("Team 10!");
		
	DATA = 0x1c;
	LCD_Banner();
}

void ADC(void){						//Lite Demo of the Analog to Digital Converter
	UART_Puts("\r\nPrinting temperature in Fahrenheit. Press any key to stop.");
	Get_Input();
}

void ReadTemp(void){
	ADC_Get();
	/*The conversion result 1023 corresponds to 5 V. Therefore, we need to divide the conversion result by 204.6 to convert it 
	back to a voltage. Because the AVR does not support floating point operation directly, we multiply the conversion result by 
	10 and then divide the product by 2046 to obtain the voltage value.
	*/
	//floor(log_2(number))+1 to calculate number of bits needed for that number
	Acc = ((int)HADC*256.0+(int)LADC);						//combine upper and lower A/D result, Acc is in bits. Times ten to begin conversion
	r = 10000.0*Acc/(1024.0-Acc);							//thermistor resistance value due to integer voltage value												
	tempk = 3950.0*298.15/(3950.0+298.15*log(r/10000.0));	//convert to temperature in kelvin
	tempc = tempk-273.15;									//convert to Celsius
	tempf = tempc*9.0/5.0+32.0;								//convert to Fahrenheit
	itoa(tempf,Tempf,10);									//convert float to ASCII

	UART_Puts("\r\n");
	UART_Puts(Tempf);
	UART_Puts(" "); 
	UART_Puts(MS6);
	UART_Puts("\r\n");
	UART_Clear();
}

void EEPROM(void)
{
	UART_Puts("\r\nEEPROM Write and Read.");
	/*
	Re-engineer this subroutine so that a byte of data can be written to any address in EEPROM
	during run-time via the command line and the same byte of data can be read back and verified after the power to
	the Xplained Mini board has been cycled. Ask the user to enter a valid EEPROM address and an
	8-bit data value. Utilize the following two given Assembly based drivers to communicate with the EEPROM. You
	may modify the EEPROM drivers as needed. User must be able to always return to command line.
	*/
	UART_Puts("\r\n");
	EEPROM_Write();
	UART_Puts("\r\n");
	EEPROM_Read();
	UART_Put();
	UART_Puts("\r\n");
}


void Command(void)					//command interpreter
{
	UART_Puts(MS3);
	ASCII = '\0';						
	while (ASCII == '\0')
	{
		UART_Get();
	}
	switch (ASCII)
	{
		case 'L' | 'l': LCD();
		break;
		case 'A' | 'a': ADC();
		break;
		case 'E' | 'e': EEPROM();
		break;
		default:
		UART_Puts(MS5);
		HELP();
		break;  			
//Add a 'USART' command and subroutine to allow the user to reconfigure the 						
//serial port parameters during runtime. Modify baud rate, # of data bits, parity, 							
//# of stop bits.
	}
}

int main(void)
{
	Mega328P_Init();
	Banner();
	while (1)
	{
		Command();				//infinite command loop
	}
}

