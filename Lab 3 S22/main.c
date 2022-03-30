
 // Lab3P1.c
 //
 // 
 // Author : Eugene Rockey
 // 
 
 //no includes, no ASF, no libraries
 
 const char MS1[] = "\r\nECE-412 ATMega328PB Tiny OS";
 const char MS2[] = "\r\nBy ECE412 Team 10: Liam Bellis, Chris Moore, Henry Reynolds, Joshua Riddell, Adam Triebsch";
 const char MS3[] = "\r\nMenu: (L)CD, (A)DC, (E)EPROM, (C)Reconfigure, (S)et Data Value at Mem, (G)et Data Value at Mem\r\n";
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

unsigned char ASCII;			//shared I/O variable with Assembly
unsigned char DATA;				//shared internal variable with Assembly
char HADC;						//shared ADC variable with Assembly
char LADC;						//shared ADC variable with Assembly

// all values below shared with assembly
unsigned short thousands;		// variable for getting terminal address
unsigned short hundreds;		// variable for getting terminal address and data value
unsigned short tens;			// variable for getting terminal address and data value
unsigned short ones;			// variable for getting terminal address and data value
unsigned short total;			// variable for getting terminal address
unsigned char firstByte;		// variable for setting EEPROM memory location
unsigned char secondByte;		// variable for setting EEPROM memory location
unsigned char dataByte;			// variable for getting EEPROM memory value
int intValue;				// variable for getting dataByte
char volts[5];				// string buffer for ADC output


char volts[5];					//string buffer for ADC output
int Acc;						//Accumulator for ADC use

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

void ADC(void)						//Lite Demo of the Analog to Digital Converter
{
	volts[0x1]='.';
	volts[0x3]=' ';
	volts[0x4]= 0;
	ADC_Get();
	Acc = (((int)HADC) * 0x100 + (int)(LADC))*0xA;
	volts[0x0] = 48 + (Acc / 0x7FE);
	Acc = Acc % 0x7FE;
	volts[0x2] = ((Acc *0xA) / 0x7FE) + 48;
	Acc = (Acc * 0xA) % 0x7FE;
	if (Acc >= 0x3FF) volts[0x2]++;
	if (volts[0x2] == 58)
	{
		volts[0x2] = 48;
		volts[0x0]++;
	}
	UART_Puts(volts);
	UART_Puts(MS6);
	/*
		Re-engineer this subroutine to display temperature in degrees Fahrenheit on the Terminal.
		The potentiometer simulates a thermistor, its varying resistance simulates the
		varying resistance of a thermistor as it is heated and cooled. See the thermistor
		equations in the lab 3 folder. User must always be able to return to command line.
	*/
	
}

void Reconfigure(void)
{
	UART_Puts("\n\rChange settings to: \n\r\tBaudrate: 4800\n\r\tBit Frame bit\n\r\tParity: Even\n\r\tStop Bit: 2\n\r");
	ChangeSettings();
}

void setMemoryAddress(void)
{
	UART_Puts("Memory location must fall between 0x0100 and 0x1024 inclusive. Memory location cannot exceed range.");
	
	UART_Puts("Please Insert the digit at the thousand's place of the wanted memory address (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	thousands = ASCII;
	
	UART_Puts("Please Insert the digit at the hundred's place of the wanted memory address (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	hundreds = ASCII;
	
	UART_Puts("Please Insert the digit at the ten's place of the wanted memory address (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	tens = ASCII;
	
	UART_Puts("Please Insert the digit at the ones's place of the wanted memory address (decimal, not hex, must be even):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	ones = ASCII;
	
	total = (thousands * 1000) + (hundreds * 100) + (tens * 10) + (ones);
	
	firstByte = (total & 0xFF);			//Extracts first byte from total
	secondByte = ((total >> 8) & 0xFF); // Extracts second byte from total
}

void setMemoryData(void)
{
	UART_Puts("Data value installed at memory location must fall within the range of 0 to 255 inclusive.");
	
	UART_Puts("Please Insert the digit at the hundred's place of the wanted data value (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	hundreds = ASCII;
	
	UART_Puts("Please Insert the digit at the ten's place of the wanted data value (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	tens = ASCII;
	
	UART_Puts("Please Insert the digit at the ones's place of the wanted data value (decimal, not hex):"); //PC Terminal
	ASCII = '\0';
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	ones = ASCII;
	
	dataByte = (hundreds * 100) + (tens * 10) + (ones);
}

void getMemoryData(void)
{	
	output[0x4] = 0;
	intValue = (int) dataByte;
	output[0x0] = intValue;
	UART_Puts(output); //PC Terminal
}

void setDataAtMemory(void)
{
	firstByte = 0;
	secondByte = 0;
	dataByte = 0;
	thousands = 0;
	hundreds = 0;
	tens = 0;
	ones = 0;
	total = 0;
	
	setMemoryAddress();
	setMemoryData();
	EEPROM_Write();
}

void getDataAtMemory(void)
{
	firstByte = 0;
	secondByte = 0;
	dataByte = 0;
	thousands = 0;
	hundreds = 0;
	tens = 0;
	ones = 0;
	total = 0;
	
	setMemoryAddress();
	EEPROM_Read();
	getMemoryData();
}

void EEPROM(void)
{
	UART_Puts("\r\nEEPROM Write and Read.");
	/*
	PART 3 OF LAB DEMO
	
	Re-engineer this subroutine so that a byte of data can be written to any address in EEPROM
	during run-time via the command line and the same byte of data can be read back and verified after the power to
	the Xplained Mini board has been cycled. Ask the user to enter a valid EEPROM address and an
	8-bit data value. Utilize the following two given Assembly based drivers to communicate with the EEPROM. You
	may modify the EEPROM drivers as needed. User must be able to always return to command line.
	*/
	
	setDataAtMemory();
	getDataAtMemory();
	
}


void Command(void)					//command interpreter
{
	UART_Puts(MS3); //PC Terminal
	ASCII = '\0';						
	while (ASCII == '\0')
	{
		UART_Get(); //PC Terminal get value
	}
	switch (ASCII)
	{
		case 'L' | 'l': LCD();
		break;
		case 'A' | 'a': ADC();
		break;
		case 'E' | 'e': EEPROM();
		break;
		case 'C' | 'c': Reconfigure();
		break;
		case 'S' | 's': setDataAtMemory();
		break;
		case 'G' | 'g': getDataAtMemory();
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

