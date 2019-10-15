/*
 * File:   main.c
 * Author: fran
 *
 * Created on September 10, 2019, 2:17 AM
 * 
*/


#include "config.h"
#include <xc.h>
#include <pic18f4620.h>

#define _XTAL_FREQ 4000000 //Defino la frecuencia del cristal, que en este caso es un oscilador interno

/*/void __interrupt() isr(void)
{   
    GIE=0;
    if(!PORTB)
    LATD3 = ~LATD3; //toggle RD3
    PORTB = PORTB; // leo PORTB para evitar missmatch
    RBIF = 0;
    GIE = 1;
}*/

void main(void) {
//    GIE = 1; // Interrupciones globales habilitadas.
//    PEIE = 1; // Interrupciones perifericas habilitadas.
//    RBIE = 1; // PORTB Interrupt Enable.
//    RBIF = 0; // Interrupt Flag down.
     
//    HABILITACION DE INTERRUPCIONES
    /*
     * OJO! -> SI PBADEN ESTA EN ON, 
     * NO PUEDO USAR EL RB4 PARA PONER MIS INTERRUPCIONES. QUEDAN 5,6 Y 7
     */

//    INTCON = 0xC8; // ver en pagina 95 del datasheet
//    RCON=0x00;
    // bits del INTCON
    // GIE/GIEH - PEIE/GIEL - TMR0IE - INT0IE - RBIE - TMR0IF - INT0IF - RBIF
    
//    SETTEO LOS PUERTOS 
    TRISB = 0xFF; // pongo el puerto B entero como input.
    TRISD = 0xF3; // defino el puerto D como entradas con salida en RD2 y RD3.  1111 0011
    LATD = 0xFF;
    __delay_ms(1000);
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 1;
    
    do{
    LATDbits.LATD2 = ~LATDbits.LATD2; // toggle RD2
    LATDbits.LATD3 = ~LATDbits.LATD3;
    __delay_ms(500); // Espero medio segundo
    }while(1); // Repito indefinidamente
    return;
}