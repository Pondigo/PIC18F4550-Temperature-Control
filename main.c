/*
 * File:   main.c
 * Author: Lenora Pondigo Santamaria
 * Mail: lenora.pondigosa@udlap.mx / lenora.pondigo@gmail.com
 * Github: Pondigo
 * Codewars: Pondigo
 * Created on 28 de octubre de 2023, 12:02 PM
 */

#include <xc.h>
#define _XTAL_FREQ 4000000

// Controls
#define POWER_IN PORTAbits.AN0
#define MMC_CONTROL PORTAbits.AN1
#define MMC_IN PORTAbits.AN2
#define SPEED1_IN PORTAbits.AN3
#define SPEED2_IN PORTAbits.RA4
#define SPEED3_IN PORTAbits.AN4

// Indicators
#define MOTOR_OUT PORTCbits.RC2
#define ON_LED PORTCbits.RC1
#define MMC_LED PORTCbits.RC4


// State bits
_Bool IS_ON = 0;
_Bool IS_MMC = 0; // Is in Manual Mode Control
unsigned char SPEED_SELECTED;

// Buttons flags
_Bool POWER_BF = 0; // Power in button flag
_Bool MMC_BF = 0; // Manual Mode Control button flag

void turn_off(void);
void configuraPWM(void);
void configurePWM(unsigned char pr2_value, unsigned char ccpr1l_value, unsigned char prescale);
void update_speed(unsigned char new_speed);
unsigned char get_current_speed(void);

void main(void) {
    // Define i/o
    TRISA = 1; // Input
    TRISC = 0; // Output
    ADCON1 = 15; // All digital
    
    // Configure PWM
    configurePWM(255, 127, 0);
    
    // Main while
    while(1) {
        // Check speed change
        unsigned char current_speed = get_current_speed();
        if(current_speed != SPEED_SELECTED){
            update_speed(current_speed);
        }
        
        if(MMC_IN && IS_ON && IS_MMC){            
            T2CONbits.TMR2ON = 1;
        }else {
            T2CONbits.TMR2ON = 0;
        }
        if(POWER_IN && !POWER_BF){
            POWER_BF = 1;
            IS_ON = !IS_ON;
            if(IS_ON) {
                ON_LED = 1;
            } else {
                ON_LED = 0;
                turn_off();                
            }
        } else if(!POWER_IN && POWER_BF){
            POWER_BF = 0;
        }
        
        if(MMC_CONTROL && IS_ON && !MMC_BF){
            MMC_BF = 1;
            IS_MMC  = !IS_MMC;
            MMC_LED = IS_MMC;
        } else if(!MMC_CONTROL && MMC_BF){
            MMC_BF = 0;
        }
    }
    
    return;
}

void configurePWM(unsigned char pr2_value, unsigned char ccpr1l_value, unsigned char prescale) {
    //Configura frecuencia del PWM a 488Hz
    //PR2 = 255;
    PR2 = pr2_value;

    //Duty cycle
    //CCPR1L = 127;
    CCPR1L = ccpr1l_value;
    CCP1CONbits.DC1B0 = 0;
    CCP1CONbits.DC1B1 = 0;

    //Configure prescale
    T2CONbits.T2CKPS = prescale;

    //Configure CCP1 as PWM
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M3 = 1;


    //LImpia el registro del TImer2 que guarda la cuenta
    TMR2 = 0;
}

void turn_off(void){
    T2CONbits.TMR2ON = 0;    
    IS_ON = 0;
    IS_MMC = 0;
    PORTC = 0;
}

void update_speed(unsigned char new_value){
    SPEED_SELECTED = new_value;
    switch(new_value) {
        case 0:
            configurePWM(255, 127, 0);
            break;
        case 1:
            configurePWM(255, 127, 1);
            break;
        case 2:
            configurePWM(255, 127, 3);
            break; 
        default:
            break;
    }    
}

unsigned char get_current_speed(void) {
    if(PORTAbits.RA3)
        return 0;
    else if(PORTAbits.RA4)
        return 1;
    return 2;
}

