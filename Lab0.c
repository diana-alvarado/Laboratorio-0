/*
 * File:   Lab0.c
 * Author: Diana Alvarado
 *
 * Created on 17 de enero de 2022, 04:51 PM
 */

// PIC16F887 Configuration Bit Settings
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//----------------------Librerias--------------------------------

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pic16f887.h>

//-------------------------Constantes
#define _XTAL_FREQ 8000000

/*
 * ----------------------------- Variables ----------------------------------
 */
unsigned char b1; //bandera j1
unsigned char b2; //bandera j2
unsigned char cont1; //contador j1
unsigned char cont2; //contador j2
unsigned char comenzar; //comenzar el conteo con botones
unsigned char iniciar; // iniciar la cuenta regresiva



/*
*------------------------------- Display --------------------------------------
 */
char tabla[4] = {
    0B00111111, //0
    0B00000110, //1
    0B01011011, //2
    0B01001111, //3
    };
    
/*
 * -------------------------- Prototipos de funciones -----------------------
 */
void setup(void); //configuraciones
char calc (char ver); //funcion del contador

/*
 * ------------------------------Interupciones----------------------------
 */
//interrupcion del puerto b 
void __interrupt()isr(void) {
    if(RBIF == 1){
        if (PORTBbits.RB2 == 0){ //botón de iniciar
            iniciar++;
        }
        else if (PORTBbits.RB0 == 0 && comenzar == 1){ //botón de jugador 1
            b1++;
            if(b1 == 1){
                cont1++;
                b1 = 0;
            }
        }
        else if(PORTBbits.RB1 == 0 && comenzar == 1){ //botón de jugador 2
            b2++;
            if (b2 == 1){
                cont2++;
                b2 = 0;
            }
        }
        RBIF = 0;
    }  
}
/*
 * ----------------------------- MAIN CONFIGURACION --------------------------- 
 */
void main (void)
{
    setup();
    while(1)
    { 
        if(iniciar == 1) //semaforo y conteo 
        {
            //PORTA = 0;
            //PORTD = 0;
            //rojo
            PORTEbits.RE0 = 1;
            PORTC = tabla[3];
            __delay_ms(1000);
            //amarillo
            PORTEbits.RE0 = 0;
            PORTEbits.RE1 = 1;
            PORTC = tabla[2];
            __delay_ms(1000);
            PORTC = tabla[1];
            __delay_ms(1000);
            //verde
            PORTEbits.RE1 = 0;
            PORTEbits.RE2 = 1;
            PORTC = tabla[0];
            __delay_ms(1000);
            //todo 0, menos la variable de iniciar el contador
            PORTEbits.RE0 = 0;
            PORTEbits.RE1 = 0;
            PORTEbits.RE2 = 0;
            PORTC = 0;
            iniciar++;
            comenzar = 1;
        }
        //funcionamiento del conteo
        if(comenzar == 1){
            PORTA = calc(cont1); 
            PORTD = calc(cont2);
        }
        //si j1 gana
        if (cont1 == 9){
            PORTC = tabla[1];
            PORTBbits.RB3 = 1;
            PORTBbits.RB4 = 0;
            __delay_ms(1000);
            iniciar = 0;
            comenzar = 0;
            cont1 = 0;
            cont2 = 0;
            PORTA = 0;
            PORTB = 0;
            PORTE = 0;
            PORTD = 0;
            PORTC = 0;
        }
        //si j2 gana
        else if(cont2 == 9){
            PORTC = tabla[2];
            PORTBbits.RB4 = 1;
            PORTBbits.RB3 = 0;
            __delay_ms(3000);
            iniciar = 0;
            comenzar = 0;
            cont1 = 0;
            cont2 = 0;
            PORTA = 0;
            PORTB = 0;
            PORTE = 0;
            PORTD = 0;
            PORTC = 0;
        }
    }
    return;
}


/*
 * -------------------------------- Funciones --------------------------------
 */
void setup(void)
{
    //Configuración entradas y salidas
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0b0111; //Puerto B1, 2 y 3 entradas
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    
    iniciar = 0;
    comenzar = 0;
    cont1 = 0;
    cont2 = 0;
    
    OPTION_REGbits.nRBPU = 0;
    WPUB = 0b0111; 
    
    PORTA = 0; //Contador Jugar 1
    PORTB = 0; //Botones e indica el ganador
    PORTC = 0; //Contador segundos
    PORTD = 0; // Contador de Jugar 2
    PORTE = 0; // Semaforo
    
    //Configuracion del oscilador 
    OSCCONbits.IRCF = 0b0111; // 8MHz
    OSCCONbits.SCS = 1;
    
    //Config. interrupcion RB
    IOCBbits.IOCB0 = 1; // pines en lo que habra interrupcion por cambio 
    IOCBbits.IOCB1 = 1; //de estado
    IOCBbits.IOCB2 = 1;
    RBIF = 0; //BANDERA 
    
    INTCONbits.GIE = 1; //habilitar interrupciones
    INTCONbits.RBIE = 1;    //activar interrupciones por cambio de estado
    INTCONbits.RBIF = 0;    //bajar la bandera
    
    
    return; 
}

//FUNCION PARA EL CONTADOR DE DÉCADAS
char calc (char ver){
    char salida;
    switch (ver){
        case (0):
            salida = 0b00000000;
            break;
        case (1):
            salida = 0b00000001;
            break;
        case (2):
            salida = 0b00000010;
            break;
        case (3):
            salida = 0b00000100;
            break;
        case (4):
            salida = 0b00001000;
            break;
        case(5):
            salida = 0b00010000;
            break;
        case (6):
            salida = 0b00100000;
            break;
        case (7):
            salida = 0b01000000;
            break;
        case (8):
            salida = 0b10000000;
            break;
    }
    return salida;
}
