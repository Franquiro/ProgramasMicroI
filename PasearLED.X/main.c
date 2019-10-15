/*
 * File:   main.c
 * Author: Francisco Quiroga y Medus
 *
 * Created on September 11, 2019, 4:16 AM
 */
#include "configPIC.h"
#include <xc.h>
#define _XTAL_FREQ 4000000
void __interrupt() isr(){
// INTERRUPCION CON BOTON
 GIE = 0;
 if(PORTB)
 {
    if(LATD == 0x00) LATD = 0x01;
    else if(LATD < 0x80) LATD = LATD*2;
    else LATD = 0x00;
 }
 PORTB = PORTB;
 RBIF = 0;
 GIE = 1;
 
/*// INTERRUPCION CON TIMER
 GIE = 0;
 if(LATD == 0x00) LATD = 0x01;
 else if(LATD < 0x80) LATD = LATD*2;
 else LATD = 0x00;
 TMR0 = 0; // esto fue un intento a lo bruto de resetear el timer, porque pense que podria ser eso lo que me estaba afectando.
 // no se si en este caso hay situacion de mismatch como en la interrupcion de portB, y en ese caso, como se limpiaria?
 TMR0IF = 0; 
 GIE = 1;*/
}

void main(void) {
    LATD = 0x00; // pongo un 0 en todos los pines del puerto D
    TRISD = 0x00; //pongo todo el puerto D como salida
    INTCON = 0xC8; // interrupciones con boton en el puerto B
//    INTCON = 0xE0; // activo interrupciones de TMR0
//    T0CON = 0x80; //activo el timer sin prescaler
    while(1);
    return;
}
