/*
 * File:   main.c
 * Author: Francisco Quiroga y Medus
 *
 * Created on September 27, 2019, 11:20 PM
 */


#include "config.h"

void __interrupt() leerADC();
void main(void) {
    /*TOMO LOS TRES PUERTOS QUE VOY A USAR Y LES CLAVO UN 0 POR LAS DUDAS*/
    LATD = 0x00;
    LATE = 0x00;
    TMR0=55536;//1MS
    
    /*DEFINO COMO VOY A USAR CADA PUERTO*/
    TRISD = 0x00;   //TODOS OUTPUTS
    TRISE = 0x0F;   // UUUU1111
    
    /* DEFINO MIS CONFIGURACIONES */
    INTCON = 0xE0;  // GIE-PEIE-TMR0IE-nINT0IE-nRBIE-nTMR0IF-nINT0IF-nRBIF
    T0CON  = 0x80;  // activo el timer sin prescaler 
    ADCON0 = 0x1D;  // U - U - CH6 - nGO - ADON 
    ADCON1 = 0x00;  // U - U - VSS - VDD - PORT<0-6>
    ADCON2 = 0x3C;  // RIGHT - U - 8_TAD - FOSC/2
    ADIE   = 1; //ACTIVO INTERRUPCIONES POR AD
    while(1);
    return;
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
//        LATD4 = 1;
    }
    else if(ADIF){
//        valor = ((ADRESH<<8) + ADRESL)/4; //lo divido entre 4 para hacerlo entrar en 255
        valor = ADRESH;
        /*
         * Ahora me fijo el valor que tengo y lo divido en 8 intervalos.
         * el ultimo no es exactamente igual porque no quise hacer la cuenta completa para ver que
         * multiplicador poner, pero son bastante iguales.
         * El ultimo multiplicador (7.8) fue arbitrario para tomar el ultimo margen. el de "completo" porque podria
         * no llegarse al 255 por ruidos. le dejé un mini margen.
         */
        if     (valor >= 7.8*255/8) LATD = 0b11111111; // 0xFF
        else if(valor > 7*255/8)    LATD = 0b01111111; // 0x7F   
        else if(valor > 6*255/8)    LATD = 0b00111111; // 0x3F
        else if(valor > 5*255/8)    LATD = 0b00011111; // 0x1F
        else if(valor > 4*255/8)    LATD = 0b00001111; // 0x0F
        else if(valor > 3*255/8)    LATD = 0b00000111; // 0x07
        else if(valor > 2*255/8)    LATD = 0b00000011; // 0x03
        else if(valor > 1*255/8 )   LATD = 0b00000001; // 0x01
        else                        LATD = 0b00000000; // 0x00
    }
    
    TMR0IF = 0; //bajo el flag del timer0
    ADIF = 0;   // bajo el flag del AD
    GIE = 1;    // reactivo las interrupciones
}