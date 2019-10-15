/*
 * File:   main.c
 * Author: root
 *
 * Created on October 1, 2019, 11:23 PM
 */


#include "initconfig.h"
#include <pic18f4620.h>
#include <stdio.h>


#define RS LATD0                   /* PORTD 0 pin is used for Register Select */
#define EN LATD1                   /* PORTD 1 pin is used for Enable */
#define ldata LATB                 /* PORTB is used for transmitting data to LCD */

#define LCD_Port TRISB              
#define LCD_Control TRISD

void LCD_Init(void);
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void LCD_String_xy(char ,char ,const char*);
void MSdelay(unsigned int );
void LCD_Num_xy(char,char,int);
void __interrupt() leerADC();



/*****************************Main Program*******************************/

void main(void)
{       
    char i,shift;
    shift  = 15;
    OSCCON = 0x72;/*use Internal Oscillator with Frequency 8MHZ*/
    LCD_Init();                         /*initialize 16x2 LCD*/ 
    LATE   = 0x00;
    TMR0   = 55536;//1MS
    
    /*DEFINO COMO VOY A USAR CADA PUERTO*/
    TRISE  = 0x0F;   // UUUU1111
    
    /* DEFINO MIS CONFIGURACIONES */
    INTCON = 0xE0;  // GIE-PEIE-TMR0IE-nINT0IE-nRBIE-nTMR0IF-nINT0IF-nRBIF
    T0CON  = 0x80; //activo el timer sin prescaler 
    ADCON0 = 0x19;  // U - U - CH6 - nGO - ADON 
    ADCON1 = 0x08;  // U - U - VSS - VDD - PORT<0-6>
    ADCON2 = 0xBC;  // RIGHT - U - 8_TAD - FOSC/2
    ADIE   = 1; //ACTIVO INTERRUPCIONES POR AD
    
    LCD_String_xy(1,3,"ELOV");
    LCD_String_xy(2,2,"Valor: ");/*display string at location(row, location).
                                         *This function passes string to display*/    
    while(1)
    {
//        for(i=0;i<shift;i++)
//        {    
//          LCD_Command(0x1c);		    /* Shift entire display to right */
//          MSdelay(1000);
//        }
//        for(i=shift;i>0;i--)
//        {    
//          LCD_Command(0x18);	        /* Shift entire display to left */
//          MSdelay(1000);
//        }
    }			
}

/****************************Functions********************************/
void LCD_Init()
{
    MSdelay(15);           /* 15ms,16x2 LCD Power on delay */
    LCD_Port = 0x00;       /* Set PORTB as output PORT for LCD data(D0-D7) pins */
    LCD_Control = 0x00;    /* Set PORTD as output PORT LCD Control(RS,EN) Pins */
    LCD_Command(0x38);     /* uses 2 line and initialize 5*7 matrix of LCD */
    LCD_Command(0x01);     /* clear display screen */
    LCD_Command(0x0c);     /* display on cursor off */
    LCD_Command(0x06);     /* increment cursor (shift cursor to right) */
}

void LCD_Clear()
{
    	LCD_Command(0x01); /* clear display screen */
}

void LCD_Command(char cmd )
{
	ldata= cmd;            /* Send data to PORT as a command for LCD */   
	RS = 0;                /* Command Register is selected */
	EN = 1;/* High-to-Low pulse on Enable pin to latch data */ 
	NOP();
	EN = 0;
	MSdelay(3);	
}

void LCD_Char(char dat)
{
	ldata= dat;            /* Send data to LCD */  
	RS = 1;                /* Data Register is selected */
	EN=1;                  /* High-to-Low pulse on Enable pin to latch data */   
	NOP();
	EN=0;
	MSdelay(1);
}


void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
    LCD_Char(*msg);
	  msg++;
    	}
		
}

void LCD_String_xy(char row,char pos,const char *msg)
{
    char location=0;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }  
    LCD_String(msg);

}

/*********************************Delay Function********************************/
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<val;i++)
            for(j=0;j<165;j++);      /*This count Provide delay of 1 ms for 8MHz Frequency */
 }

/////////////////////////////////////////////////////////////////////////////////
// INTERRUPCION
/////////////////////////////////////////////////////////////////////////////////
void __interrupt() leerADC(){
    GIE = 0;
    int valor = 0; //el int tiene 16 bits, entran los dos registros
    /* TMR0IF se activa cada 1ms, entonces arranca a muestrear el valor del adc.
     * cuando termina de calcularlo, el bit del registro GO se pone en 0 y se levanta
     * el bit de ADIF(Analog to Digital Interrupt Flag)
     * Vuelvo a entrar en la interrupcion pero ahora por el ADC y voy al else donde
     * calculo cuantos leds prender.
    */
    if(TMR0IF){
        ADCON0bits.GO = 1;
        TMR0 = 55536;
    }
    else if(ADIF){
//        valor = (((ADRESH<<8) + ADRESL)/4) -128; //lo divido entre 4 para hacerlo entrar en 255
        valor = 5*(ADRESH<<8+ADRESL)*100/1024;
        //LCD_String_xy(2,8,"hola");
        LCD_Num_xy(1,2,127);
    }
    
    TMR0IF = 0; //bajo el flag del timer0
    ADIF = 0;   // bajo el flag del AD
    GIE = 1;    // reactivo las interrupciones
}

void LCD_Num_xy(char row,char pos,int msg)
{
    char location=0;
    char mensaje[5] = {0,0,0,0,'\0'};
    int i;
    int aux = msg;
    unsigned int mask = 0x8000;
    if(row<=1)
    {
        location=(0x80) | ((pos) & 0x0f); /*Print message on 1st row and desired location*/
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); /*Print message on 2nd row and desired location*/
        LCD_Command(location);    
    }
    
    
    ///OJO QUE PUEDE QUE EL NUMERO TENGA MENOS DE 3 DIGITOS Y NO LO PUEDO SACAR
    ///ALEGREMENTE ASI POR EL SIGNO.
    for(i=3 ;i > 0; i--){
        mensaje[i] = (msg%10) + 48;
        msg = msg/10;
    }
    
    if(aux & mask) mensaje[0] = '-';
    else mensaje[0]='+';
    
    LCD_String(mensaje);
}