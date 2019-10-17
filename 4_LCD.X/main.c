/*
 * File:   LCD
 * Author: Francisco Quiroga y Medus
 *
 * Created on October 8, 2019, 2:34 AM
 */

/*  DATA DEL LCD
 *  http://hades.mech.northwestern.edu/images/f/f7/LCD16x2_HJ1602A.pdf
 */
#include "config.h"
#include <xc.h>

#define RS LATD2        // PIN 2 del PUERTO D se asigna al pin <register select> del LCD
#define EN LATD3        // PIN 3 del PUERTO D se asigna al pin <enable> del LCD 
#define ldata LATD      // PORTD(PD4-PD7) asignado al LCD Data Output
#define LCD_Port TRISD  //Puerto D es el puerto del LCD

/*Declaracion de prototipos*/

void MSdelay(unsigned int );                    // delay en milisegundos*/
void LCD_Init();                                // Inicializar LCD*/
void LCD_Command(unsigned char );               // mandar comando al lcd*/
void LCD_Char(unsigned char x);                 // mandar un char al lcd*/
void LCD_String(const char *);                  // mandar un string al lcd*/
void LCD_String_xy(char, char , const char *);  // Manda un string con la posicion
void LCD_Clear();                               // Limpiar el LCD


int main(void)
{    
    OSCCON = 0x72;  // Utilizar el oscilador interno, frecuencia 8MHz 
	LCD_Init();     // Inicializar el LCD    
    LATE   = 0x00;  // Puerto E utilizado para el ADC (AN6 o AN7)
    TMR0   = 55536; // 1ms
    
    /*DEFINO COMO VOY A USAR CADA PUERTO*/
    TRISE  = 0x0F;   // UUUU1111
    
    /* DEFINO MIS CONFIGURACIONES */
    INTCON = 0xE0;  // GIE-PEIE-TMR0IE-nINT0IE-nRBIE-nTMR0IF-nINT0IF-nRBIF
    T0CON  = 0x80; //activo el timer sin prescaler 
    ADCON0 = 0x19;  // U - U - CH6 - nGO - ADON 
    ADCON1 = 0x08;  // U - U - VSS - VDD - PORT<0-6>
    ADCON2 = 0xBC;  // RIGHT - U - 20_TAD - FOSC/4
    ADIE   = 1; //ACTIVO INTERRUPCIONES POR AD
    
    /* ARRANCA EL PROGRAMA EN SI */
	LCD_String_xy(1,1,"Lectura ADC");  // Muestro el string en la primer fila, primer columna
    LCD_String_xy(2,3,"Valor");  // Muestro el string en la segunda fila, tercer columna           	
	while(1);		
}

/**************************** FUNCIONES ********************************/

void LCD_Init()
{
    LCD_Port = 0;       //Puerto D como salida
    MSdelay(15);        // delay de 15ms
    LCD_Command(0x02);  // Inicialización del LCD en modo <nibble> (4 bits)
    LCD_Command(0x28);  // Usar 2 lineas e inicializar matriz de 5*8 en modo nibble
	LCD_Command(0x01);  // Limpiar el display
    LCD_Command(0x0c);  // sacar el cursor
	LCD_Command(0x06);  // Incrementar el cursor (mover a derecha)	   
}

void LCD_Command(unsigned char cmd )
{
    /* MANDA LA INFORMACIÓN EN MEDIOS BYTES (nibbles) EN DOS PARTES. */
	/* PARTE ALTA DE LA INFO */
    ldata = (ldata & 0x0f) |(0xF0 & cmd);  // Manda la parte alta de la info 
	RS = 0;  // Selecciono el registro de comando RS=0	
    
    /* Mando un pulso <High-to-low> en el pin de <ENABLE> para fijar los datos */
    EN = 1;  // HIGH
    NOP();   // HOLD
    EN = 0;  // LOW
	MSdelay(1); // espero. Es importante esperar entre mandado de nibbles.
    
    /* PARTE BAJA DE LA INFO*/
    ldata = (ldata & 0x0f) | (cmd<<4);  // mando la parte baja de la info
	EN = 1;     // HIGH
	NOP();      // HOLD
	EN = 0;     // LOW
	MSdelay(3); // espero
}


void LCD_Char(unsigned char dat)
{  
//    Esta función es igual a LCD_Command() con el RS cambiado
    
	ldata = (ldata & 0x0f) | (0xF0 & dat); 
	RS = 1;  // Register Select se pone en 1 para escribir y leer datos. para todo lo de mas va en 0
	EN = 1;  
	NOP();
	EN = 0;
	MSdelay(1);
    ldata = (ldata & 0x0f) | (dat<<4);  
	EN = 1;  
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
     for(j=0;j<165;j++);  // bucle de 1ms para 8MHz
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
        location=(0x80) | ((pos) & 0x0f); // Fila 1, columna deseada
        LCD_Command(location);
    }
    else
    {
        location=(0xC0) | ((pos) & 0x0f); // Fila 2, columna deseada
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