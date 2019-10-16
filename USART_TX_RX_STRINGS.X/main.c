/*
 * File:   main.c
 * Author: Francisco Quiroga y Medus
 *
 * Created on October 15, 2019, 12:12 AM
 */


#include "config.h"
#define baud 9600
#define ON_STATE LATD2


void USART_config(long Baud_Rate);
void USART_send_char(unsigned char salida);
char USART_receive_char(void);
void USART_receive_string(char data[20]);
void USART_send_string(char data[20]);
void main(void){
    char dato;
    char data[20] = "USART Iniciada...\0";
    TRISD = 0xFB;
    ON_STATE = 1;
    USART_config(baud);
    __delay_ms(50);
    USART_send_string(data);
    
    while(1){
        USART_receive_string(data);
        USART_send_string(data);
    }
    
}
void USART_config(long Baud_Rate){
    TRISC = 0xBF;     
    SPBRG = (int) ((((float)_XTAL_FREQ/(float)Baud_Rate)/64)-1);     
    TXSTA = 0x20;       
    RCSTA = 0x90;
    BAUDCON = 0x00;
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
void USART_send_string(char data[20]){
    int i = 0;
    while(i<20 && data[i] != '\0'){
        USART_send_char(data[i]);
        i++;
    }
    USART_send_char('\r');
}
void USART_receive_string(char data[20]){
    int i = 0;
    char dato;
    while(!RCIF);
    while((dato=USART_receive_char()) && i< 20){
        if(dato == '\r' || dato == '\n') break;
        data[i] = dato;
        i++;
    }
    if(i<20) data[i] = '\0';
}