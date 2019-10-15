/*
 * File:   funciones.c
 * Author: Francisco Quiroga y Medus
 *
 * Created on October 12, 2019, 4:25 AM
 */
#include<xc.h>

void USARTconfig(void){
    /*SETUP ASYNC TX*/
//1. Initialize the SPBRGH:SPBRG registers for the appropriate baud rate. 
//    Set or clear the BRGH and BRG16 bits, as required, to achieve the desired baud rate.
//      TXSTA: CSRC - TX9 - TXEN - SYNC - SENDB - <BRGH> - TRMT - TX9D (1-H,0-L)
//2. Enable the asynchronous serial port by clearing bit SYNC and setting bit SPEN.
//      TXSTA: CSRC - TX9 - TXEN - <SYNC> - SENDB - 1 - TRMT - TX9D
//3. If interrupts are desired, set enable bit TXIE. 
//    PIE1: PSPIE - ADIE - RCIE - <TXIE> - SSPIE - CCP1IE - TMRE2IE - TMR1IE
//4. If 9-bit transmission is desired, set transmit bit TX9. Can be used as address/data bit.
//      TXSTA: CSRC - <TX9> - TXEN - 0 - SENDB - 1 - TRMT - TX9D   
//5. Enable the transmission by setting bit TXEN which will also set bit TXIF.
//      TXSTA: CSRC - 0 - <TXEN> - 0 - SENDB - 1 - TRMT - TX9D
//6. If 9-bit transmission is selected, the ninth bit should be loaded in bit TX9D.
//      TXSTA: CSRC - 0 - 1 - 0 - SENDB - 1 - TRMT - <TX9D>
//7. Load data to the TXREG register (starts transmission).

//8. If using interrupts, ensure that the GIE and PEIE bits in the INTCON register (INTCON<7:6>) are set.
    TRISC = 0xBF;     // 0b10111111 todos Inputs, solo necesito los dos de TX y RX en realidad.
    SPBRG = 0xFF;       // Valor 25, sacado por tabla de velocidad con BRGH = 1
    TXSTA = 0x24;       // CSRC - TX9 - TXEN - SYNC - SENDB - BRGH - TRMT - TX9D
//                     CSRC (clock source)     = 0 porque en asíncrono no me interesa un clock.
//                     TX9  (transmitir 9 bits)= 0 porque solo quiero transmitir 8
//                     TXEN (transmit enable)  = 1 para habilitar la transmisión de datos por USART
//                     SYNC (síncrono)         = 0 porque lo voy a usar de manera asíncrona
//                     SENDB (send break)      = 0 no me interesa mandar un caracter de break.
//                     BRGH (BRG High)         = 1 para generar baudrates de alta velocidad.
//                     TRMT (Estado del reg)   = 0 cuando está en 1, está vacío
//                     TX9D (9no bit)          = 0 solo me interesa si transmito 9 bits
                    
//    RCSTA = 0x90;     // SPEN - RX9 - SREN - CREN - ADDEN - FERR - OERR - RX9D
    RCSTA = 0x90;
//                    SPEN (SerialPort Enable)= 1
//                    RX9 (Recibir 9 bits)    = 0
//                    SREN(Single Receive)    = 0
//                    CREN(Cointinuous Rec)   = 0
//                    ADDEN(Address detect)   = 0
//                    FERR (Framing Error)    = 0
//                    OERR (Overrun Error)    = 0
//                    RX9D (9no bit recibido) = 0
                 
    BAUDCON = 0x40;     // ABDOVF - RCIDL - U - SCKP - BRG16 - U - WUE - ABDEN
//                     ABDOF (Auto baud OF)   = 0 Se pone solo en 1 cuando hay un overflow en la adquisisión del baud
//                     RCIDL (Operation Idle) = 1 (0 para activo, 1 para idle)
//                     SIN IMPLEMENTAR        = 0
//                     SCKP (polaridad clock) = 0 en asíncrono no importa, porque no se usa
//                     BRG16 (BRG de 16-bit)  = 0 mantengo un BRG de 8 bits
//                     SIN IMPLEMENTAR        = 0
//                     WUE (despertar hab)    = 0
//                     ABDEN (auto baut en)   = 0 toma el baud rate automaticaente.
   INTCON = 0xC0;
   PIR1=0x00;
   PIE1 = 0x00;
   
}
