/*
 * File:   main.c
 * Author: root
 *
 * Created on October 8, 2019, 2:34 AM
 */
//https://deepbluembedded.com/interfacing-16x2-lcd-with-pic-microcontrollers-mplab-xc8/
//https://www.electronicwings.com/pic/lcd16x2-interfacing-with-pic18f4550

#include "config.h"
#include <xc.h>
#include <pic18f4620.h>

#define RS LATD2  /*PIN 0 of PORTB is assigned for register select Pin of LCD*/
#define EN LATD3  /*PIN 1 of PORTB is assigned for enable Pin of LCD */
#define ldata LATD  /*PORTB(PB4-PB7) is assigned for LCD Data Output*/ 
#define LCD_Port TRISD  /*define macros for PORTB Direction Register*/

/*********************Proto-Type Declaration*****************************/

void MSdelay(unsigned int );       /*Generate delay in ms*/
void LCD_Init();                   /*Initialize LCD*/
void LCD_Command(unsigned char );  /*Send command to LCD*/
void LCD_Char(unsigned char x);    /*Send data to LCD*/
void LCD_String(const char *);     /*Display data string on LCD*/
void LCD_String_xy(char, char , const char *);
void LCD_Clear();                  /*Clear LCD Screen*/


int main(void)
{    
    OSCCON = 0x72;  /*Use internal oscillator and 
                      set frequency to 8 MHz*/ 
	LCD_Init();  /*Initialize LCD to 5*8 matrix in 4-bit mode*/    
    LATE   = 0x00;
    TMR0   = 55536;//1MS
    
    /*DEFINO COMO VOY A USAR CADA PUERTO*/
    TRISE  = 0x0F;   // UUUU1111
    
    /* DEFINO MIS CONFIGURACIONES */
    INTCON = 0xE0;  // GIE-PEIE-TMR0IE-nINT0IE-nRBIE-nTMR0IF-nINT0IF-nRBIF
    T0CON  = 0x80; //activo el timer sin prescaler 
    ADCON0 = 0x19;  // U - U - CH6 - nGO - ADON 
    ADCON1 = 0x08;  // U - U - VSS - VDD - PORT<0-6>
    ADCON2 = 0xBC;  // RIGHT - U - 20_TAD - FOSC/4
    ADIE   = 1; //ACTIVO INTERRUPCIONES POR AD
	LCD_String_xy(1,1,"Lectura ADC");  /*Display string on 1st row, 5th location*/
    LCD_String_xy(2,3,"Valor");  /*Display string on 2nd row,1st location*/           	
	while(1);		
}

/****************************Functions********************************/

void LCD_Init()
{
    LCD_Port = 0;       /*PORT as Output Port*/
    MSdelay(15);        /*15ms,16x2 LCD Power on delay*/
    LCD_Command(0x02);  /*send for initialization of LCD 
                          for nibble (4-bit) mode */
    LCD_Command(0x28);  /*use 2 line and 
                          initialize 5*8 matrix in (4-bit mode)*/
	LCD_Command(0x01);  /*clear display screen*/
    LCD_Command(0x0c);  /*display on cursor off*/
	LCD_Command(0x06);  /*increment cursor (shift cursor to right)*/	   
}

void LCD_Command(unsigned char cmd )
{
	ldata = (ldata & 0x0f) |(0xF0 & cmd);  /*Send higher nibble of command first to PORT*/ 
	RS = 0;  /*Command Register is selected i.e.RS=0*/ 
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/ 
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (cmd<<4);  /*Send lower nibble of command to PORT */
	EN = 1;
	NOP();
	EN = 0;
	MSdelay(3);
}


void LCD_Char(unsigned char dat)
{  
	ldata = (ldata & 0x0f) | (0xF0 & dat);  /*Send higher nibble of data first to PORT*/
	RS = 1;  /*Data Register is selected*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (dat<<4);  /*Send lower nibble of data to PORT*/
	EN = 1;  /*High-to-low pulse on Enable pin to latch data*/
	NOP();
	EN = 0;
	MSdelay(3);
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
        location=(0x80) | ((pos) & 0x0f);  // muestra el mensaje en la primer fila, columna que le pida
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f);  // muestra el mensaje en la segunda fila, columna que le pida
        LCD_Command(location);    
    }  
    

    LCD_String(msg);

}
void LCD_Clear()
{
   	LCD_Command(0x01);  // Limpia el display del lcd
    MSdelay(3);
}

void MSdelay(unsigned int val)
{
 unsigned int i,j;
 for(i=0;i<val;i++)
     for(j=0;j<165;j++);  /*This count Provide delay of 1 ms for 8MHz Frequency */
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
    for(i=3 ;i >= 0; i--){
        mensaje[i] = (msg%10) + 48;
        msg = msg/10;
    }
    
//    if(aux & mask) mensaje[0] = '-';
//    else mensaje[0]='+';
    
    LCD_String(mensaje);
}

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
        valor = (((ADRESH<<8) + ADRESL)); //lo divido entre 4 para hacerlo entrar en 255
        //valor = 5*(ADRESH<<8+ADRESL)*100/1024;
        //LCD_String_xy(2,8,"hola");
        LCD_Num_xy(2,9,valor);
    }
    
    TMR0IF = 0; //bajo el flag del timer0
    ADIF = 0;   // bajo el flag del AD
    GIE = 1;    // reactivo las interrupciones
}