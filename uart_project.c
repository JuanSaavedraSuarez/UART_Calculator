#include <reg51.h>

/*************************/
/*	Global Varialbes		 */
/*************************/

char rxBuffer [ 4 ];
char count;
short decVal ;

/*************************/
/*	Function Prototypes	 */
/*************************/

void UART0_Init_8bit_Timer1_9600();
void sendByte( char );
void sendString( char [] );
void send4DigitDecimal( short );
char getByte();
char getString( char * );
short asciiToShort( char [], char );
// input processing functions:
short getShort();
void printOp(short, short , char );
// arithmetic functions
void add();
void sub();
void mul();
void div();


/*************************/
/*	Main Function   		 */
/*************************/

void main()
{
	UART0_Init_8bit_Timer1_9600();
	sendString("Welcome to the 8051 Menu Based Calculator \n\n");
	while(1)
	{
		sendString("Please select an option: \n\n1. Addition \n2. Subtraction \n3. Multiplication \n4. Division \n\nEnter your choice: ");
		count = getString( rxBuffer );
		sendString( rxBuffer);
		sendByte('\n');
		decVal = asciiToShort(rxBuffer, count);
		switch (decVal)
		{
			case 1:
				add();
				break;
			case 2:
				sub();
				break;
			case 3:
				mul();
				break;
			case 4:
				div();
				break;
			default:
				sendString("Invalid input\n");
				break;
		}
		//send4DigitDecimal( decVal);
	}
}

/*************************/
/*  Function Declarations*/
/*************************/

/*
	UART0_Init_8bit_Timer1_9600: initializes UART peripheral
		non interrupt based functionality
	receives: nothing
	returns: nothing
*/

void UART0_Init_8bit_Timer1_9600()
{
	TMOD 	=	0x20;			// 8-bit auto-reload Timer1
	TH1 	= 0xFA;			// 9600 Baud
	SCON	= 0x50;			// 8-bit data, Timer 1 sets speed, Reception Enabled
	TR1 	= 1		;			// start Timer1
}

/*
	sendByte: send a single byte using UART
	receives: the byte to be sent
	returns: nothing
*/
void sendByte( char Byte)
{
	SBUF = Byte;				// send A
	while( TI == 0 );		// wait until it is sent
	TI = 0;							// clear timer flag
}


void sendString( char Bytes[])
{
	int i;
	for( i = 0; Bytes [ i ] != 0; i++)
	{
		sendByte( Bytes[ i ]);
	}
}
/*
	send4DigitDecimal: transmits a zero filled 4 digit decimal
	receives: 16-bit data to be treated as a decimal quantity
	returns: nothing
*/

void send4DigitDecimal( short decVal)
{
	sendByte( ( decVal/1000)        | 0x30 );
	sendByte( ((decVal/100 ) % 10 ) | 0x30 ); 
	sendByte( ((decVal/10  ) % 10 ) | 0x30 );
	sendByte( ((decVal     ) % 10 ) | 0x30 );
}

/*
	getByte: receive on byte from UART0
		non interrupt based functionality
	receives: nothing
	returns: Byte char received from UART0
*/
char getByte()
{
	char myByte;
	while(RI == 0);		// wait until a byte is received
	myByte = SBUF;		// read byte from serial buffer
	RI = 0;						// clear Rx flag
	return myByte;
}

/*
	getString: receive bytes from UART0 until carriabe return is found
	receives: reference to array / buffer
	returns: nothing explicitly but overwtrites referenced array / buffer
*/

char getString( char *buffer)
{
	char myByte;
	char count = 0;
	do{
		myByte = getByte();
		if( myByte != '\r')
		{
			*buffer = myByte;
			buffer++;
			count++;
		}
	}
	while( myByte != '\r' );
	return count;
}

/*
	asciiToShort: converts 4 ascii numeric digit array to a 16-bit decimal quantity
	receives: array of 4 ascii numeric digits
	returns: 16-bit decimal quantity
*/

short asciiToShort( char ascii [],char count )
{
	short i, decimalValue = 0;
	// for(i = 0; i < 4; i++)
	for (i = 0; i < count; i++)
	{
		decimalValue *= 10;
		decimalValue += ( ascii[ i ] & 0x0F);
	}
	return decimalValue;
}

short getShort()
{
	count = getString( rxBuffer );
	sendString( rxBuffer );
	sendByte('\n');
	return asciiToShort(rxBuffer, count);
}

void printOp(short s1, short s2, char symbol)
{
	send4DigitDecimal(s1);
	
	sendByte(' ');
	sendByte(symbol);
	sendByte(' ');
	
	send4DigitDecimal(s2);
	sendString(" = ");
}

void add()
{
	short s1, s2;
	sendString("Please enter the augend: ");
	s1 = getShort();
	
	sendString("Please enter the addend: ");
	s2 = getShort();
	
	printOp(s1, s2, '+');
	send4DigitDecimal( s1 + s2);
	sendByte('\n');
}

void sub()
{
	short s1, s2;
	sendString("Please enter the minuend: ");
	s1 = getShort();
	
	sendString("Please enter the subtrahend: ");
	s2 = getShort();
	
	printOp(s1, s2, '-');
	send4DigitDecimal( s1 - s2);
	sendByte('\n');
}

void mul()
{
	short s1, s2;
	sendString("Please enter the multiplicand: ");
	s1 = getShort();
	
	sendString("Please enter the multiplier: ");
	s2 = getShort();
	
	printOp(s1, s2, '*');
	send4DigitDecimal( s1 * s2);
	sendByte('\n');
}

void div()
{
	short s1, s2;
	sendString("Please enter the dividend: ");
	s1 = getShort();
	
	sendString("Please enter the divisor: ");
	s2 = getShort();
	
	printOp(s1, s2, '/');
	send4DigitDecimal( s1 / s2);
	sendByte('\n');
	
	printOp(s1, s2, '%');
	send4DigitDecimal( s1 % s2);
	sendByte('\n');
}