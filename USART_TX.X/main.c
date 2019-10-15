/*
 * File:   main.c
 * Author: root
 *
 * Created on October 12, 2019, 4:25 AM
 */

#include "config.h"
#define baud 9600
#define ON_STATE LATD2


void USART_config(long Baud_Rate);
void USART_send_char(unsigned char salida);
char USART_receive_char(void);
//char* USART_receive_string(void);
void main(void){
    char dato;
//    int i = 0;
//    char*datos = "";
    TRISD = 0xFB;
    ON_STATE = 1;
    USART_config(baud);
    __delay_ms(50);
    while(1){
//        datos = USART_receive_string();
        dato = USART_receive_char();
        USART_send_char(dato);
//        USART_send_char(10);
//        USART_send_char(13);
    }
}
void USART_config(long Baud_Rate){
    TRISC = 0xBF;     
    SPBRG = (int) ((((float)_XTAL_FREQ/(float)Baud_Rate)/64)-1);     
    TXSTA = 0x20;       
    RCSTA = 0x90;
    BAUDCON = 0x00;
    RCIF = 0;
    
}

void USART_send_char(unsigned char salida){
    while(!TXIF);
    if(salida == '\r'){
        TXREG = '\n';
    }
    TXREG = salida;
    __delay_ms(5);
}

char USART_receive_char(){
    while(!RCIF);
    if(OERR){
        CREN = 0;
        NOP();
        CREN=1;
    }
    return RCREG;
    
}
//char* USART_receive_string(){
//    char* data = "";
//    int i = 0;
//    char dato;
//    
//    dato = USART_receive_char();
//    while(dato != '\0'){
//        *(data+i) = dato;
//        i++;
//    }
//    *(data+i) = '\0';
//    return data;
//}



////void send_string(char data[]);
////void interruptConfig(void);
//void __interrupt() isr();
//
//void main(void) {
//    USARTconfig();
//    
//    while(1){
//        send_char('a');
//        __delay_ms(1000);
//        TXREG = '\n';
////        __delay_ms(1000);
//        TXREG = '\r';
////        __delay_ms(1000);
//    }
//    return;
//}
//
////void interruptConfig(){
////    TRISB = 0x80;
////    LATB = 0x00;
////    INTCON = 0xC8;  // GIE/GIEH - PEIE/GIEL - TMR0IE - INT0IE - RBIE - TMR0IF - INT0IF - RBIF
////}
////
////void __interrupt() isr(){
////    GIE = 0;
////    if(PORTB)
////        send_string("Hola\r\n\0");
////    RBIF = 0;
////    LATB = PORTB;
////    GIE = 1;
////} 

//void send_char(unsigned char salida){
//    while(!TXIF);
//    TXREG = salida;
//    
//}
//void send_string(char data[]){
//    int i,longitud;
//    for(longitud = 0 ; data[longitud] != '\0' ; longitud++);
//    for(i = 0 ; i < longitud ; i++){
//        send_char(data[i]);
//        __delay_ms(10);
//    }
////    TXREG = 13; // carriage return
////    __delay_ms(10);
////    TXREG = 10; // Line Feed 
////    __delay_ms(10);
//}