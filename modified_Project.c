/*
 * Author: Harsh Dubey
*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "newfile.h"

//Configurating bits
#define _XTAL_FREQ  8000000
#pragma config PBADEN = OFF//PORTB as digital Inputs
#pragma config FOSC = HSMP
#pragma config FCMEN = OFF
#pragma config WDTEN = OFF

//TIMER VARIABLE
int COUNT = 1;
int DATAPOINTS = 0;///

unsigned char ADDHIGH = 0X00;
unsigned char HOUR = 0X00;
unsigned char HOUR_1 = 0X00;
unsigned char HOUR_2 = 0X00;

unsigned char MINS = 0X00;
unsigned char MINS_1 = 0X00;
unsigned char MINS_2 = 0X00;

int EEPint;
int EEPint_1;
int EEPint_2;
int EEPint_3;
int EEPint_4;

int EEPext;
int EEPext_1;
int EEPext_2;
int EEPext_3;
int EEPext_4;

int TempDiff;
int TempDiff_1;
int TempDiff_2;
int TempDiff_3;

unsigned char EXT_SENSOR1;
unsigned char EXT_SENSOR2;
unsigned char EXT_SENSOR3;
unsigned char EXT_SENSOR4;
unsigned char INT_SENSOR1;
unsigned char INT_SENSOR2;
unsigned char INT_SENSOR3;
unsigned char INT_SENSOR4;
int EXT_SENSOR;
int INT_SENSOR;
char EXT_SENSOR_;
char INT_SENSOR_;



void LCDINITIALIZE();
void I2CINIT();
char EEPROMWRITE(char SLAVEADDRESS, char ADDRESSHIGH, char ADDRESSLOW, char EEPROMDATA);
char EEPROMREAD(char SLAVEADDRESS, char ADDRESSHIGH, char ADDRESSLOW);
void TIMERandEXT();
char ADC_READING(char INT_SENSOR_ , char EXT_SENSOR_);

void main()
{
    LCDINITIALIZE();
    lcdCommand(0X01);
    //Timer initialize
    TIMERandEXT();
    //
    I2CINIT();
    //Timer ON
    T0CONbits.TMR0ON = 1;

    while(1);
}

void interrupt ISR()
{
    if(INTCONbits.TMR0IF == 1)
    {
   TMR0H = 0XE1;//0X66;
    TMR0L = 0X4D;//0XB4;
    COUNT--;
    if(COUNT == 0)
    {
        ADC_READING();
        if(DATAPOINTS == 256)
            {
                ADDHIGH = 0x01;
            }

        PORTEbits.RE0 = 1;
        EEPROMWRITE(0xA0, ADDHIGH, DATAPOINTS,   INT_SENSOR);
        __delay_ms(5);
        EEPROMWRITE(0xA0, ADDHIGH, DATAPOINTS+1,  EXT_SENSOR);
        __delay_ms(5);
        EEPint = EEPROMREAD(0xA0, ADDHIGH, DATAPOINTS);
        __delay_ms(5);
        EEPext = EEPROMREAD(0xA0, ADDHIGH, DATAPOINTS+1);
        __delay_ms(5);

        LCDINITIALIZE();//Replace

        lcdChar('I');
        lcdChar('N');
        lcdChar('T');
        lcdChar(':');
        EEPint_1 = EEPint%10;//3
        EEPint_2 = EEPint/10;
        EEPint_3 = EEPint_2%10;//2
        EEPint_4 = EEPint_2/10;//1
        lcdChar(EEPint_4+48);
        lcdChar(EEPint_3+48);
        lcdChar(EEPint_1+48);
        lcdChar(111);
          lcdChar('F');


        lcdGoTo(0x40);
        lcdChar('E');
        lcdChar('X');
        lcdChar(':');
        EEPext_1 = EEPext%10;//3
        EEPext_2 = EEPext/10;
        EEPext_3 = EEPext_2%10;//2
        EEPext_4 = EEPext_2/10;//1
        lcdChar(EEPext_4+48);
        lcdChar(EEPext_3+48);
        lcdChar(EEPext_1+48);
        lcdChar(111);
          lcdChar('F');

        DATAPOINTS = DATAPOINTS+2;
        COUNT = 1;

        I2CINIT();//replace

    }
    if(DATAPOINTS == 288)
    {
    __delay_ms(2000);
    ADDHIGH = 0x00;
    LCDINITIALIZE();

    lcdCommand(0X01);
    lcdChar('D');
    lcdChar('O');
    lcdChar('N');
    lcdChar('E');
    lcdChar('!');
    T0CONbits.TMR0ON = 0;
   TMR0H = 0XE1;//0X66;
    TMR0L = 0X4D;//0XB4;

    PORTEbits.RE0 = 0;//  Turn off Red LED
    DATAPOINTS = 0;
    INTCONbits.INT0IE = 1;
    }
    INTCONbits.TMR0IF = 0;
    return;
    }
    else if(INTCONbits.INT0IF == 1)
    {

        for(int j = 0; j<288; j=j+2)
        {
            if(j == 256)
            {
                ADDHIGH = 0x01;
            }
        I2CINIT();
        EEPint = EEPROMREAD(0xA0, ADDHIGH, j);
        __delay_ms(5);
        EEPext = EEPROMREAD(0xA0, ADDHIGH, j+1);
        __delay_ms(5);

        MINS = MINS +5;
        if(MINS == 60)
        {
            HOUR++;
            MINS = 0;
        }
        MINS_1 = MINS%10;
        MINS_2 = MINS/10;
        HOUR_1 = HOUR%10;
        HOUR_2 = HOUR/10;

        /* if (EXT > INT)
         {
           TempDiff = EXT - INT;

         }
         else{

          TempDiff = INTT - EXT;
         }*/

        TempDiff= 104-80;

        TempDiff_1 = TempDiff%10;
        TempDiff_2= TempDiff/10;

        LCDINITIALIZE();
        lcdChar('T');
        lcdChar('I');
        lcdChar('M');
        lcdChar('E');
        lcdChar(':');
        lcdChar(HOUR_2+48);
        lcdChar(HOUR_1+48);
        lcdChar(':');
        lcdChar(MINS_2+48);
        lcdChar(MINS_1+48);
          lcdChar(' ');
        lcdChar('D');
        lcdChar('F');
        lcdChar(':');
        lcdChar(TempDiff_2+48);
        lcdChar(TempDiff_1+48);

//        lcdChar(' ');
//        lcdChar(' ');
//        lcdChar('M');
//        lcdChar('I');
//        lcdChar('N');
//        lcdChar('S');
//        lcdChar(':');
//        lcdChar(MINS_2+48);
//        lcdChar(MINS_1+48);

        lcdGoTo(0x40);
        lcdChar('I');
        lcdChar('N');
        //lcdChar('T');
        lcdChar(':');
        EEPint_1 = EEPint%10;//3
        EEPint_2 = EEPint/10;
        EEPint_3 = EEPint_2%10;//2
        EEPint_4 = EEPint_2/10;//1
        lcdChar(EEPint_4+48);
        lcdChar(EEPint_3+48);
        lcdChar(EEPint_1+48);
        lcdChar(' ');
        lcdChar(' ');
        lcdChar('E');
        lcdChar('X');
        //lcdChar('T');
        lcdChar(':');
        EEPext_1 = EEPext%10;//3
        EEPext_2 = EEPext/10;
        EEPext_3 = EEPext_2%10;//2
        EEPext_4 = EEPext_2/10;//1
        lcdChar(EEPext_4+48);
        lcdChar(EEPext_3+48);
        lcdChar(EEPext_1+48);
         lcdChar(111);
          lcdChar('F');
        __delay_ms(1500);//////speed of displaying temp
        if(j == 286)
            {
                ADDHIGH = 0x00;
                HOUR = 0X00;
                MINS = 0X00;
            }
        }
//       INTCON3bits.INT1IE = 1;

       INTCONbits.INT0IF = 0;
       return;
    }

    INTCONbits.TMR0IF = 0;
    INTCONbits.INT0IF = 0;
    return;
}
void I2CINIT()
{
    ANSELC = 0;//PORTC DIGITAL
    TRISCbits.RC3 = 1;//SCL
    TRISCbits.RC4 = 1;//SDA
    //FOR DEBUGGING
    TRISD = 0;
    PORTD = 0;
    //CLCOK FREQUENCY = 100KHZ, FOSC/(4*(SSPADD+1)), FOSC = 8MHZ
    SSPADD = 9; // 8MHZ/(4*(9+1))
    //LOAD I2C CONTROL REGISTER
    SSPCON1 = 0X28;//MASTER = 1000, SPEN =1

}

char EEPROMWRITE(char SLAVEADDRESS, char ADDRESSHIGH, char ADDRESSLOW, char EEPROMDATA)
{

    //TRANSMISSION OF A BYTE STARTS HERE
    //START//-------------- NUMBER 1
    SSPCON2bits.SEN =1;
    while(SSPCON2bits.SEN);//WAIT FOR START CONDITION TO COMPLETE
    PIR1bits.SSPIF =0;//CLEAR INTERRUPT FLAG

    //LOAD THE EEPROM ADDRESS
    SSPBUF = SLAVEADDRESS;//LOAD SLAVE ADDRESSES TP SSPBUF REGISTER
    while(!PIR1bits.SSPIF);//WAIT FOR THE ADDRESS TO BE WRITTEN IN SSPBUF REGISTER
    PIR1bits.SSPIF = 0;//CLEAR THE INTERRUPT FLAG THAT WAS RAISED BY WRITNG SSPBUF
    if(SSPCON2bits.ACKSTAT)//CHECK IF ACK OR NACK, NACK ==1
    {
        SSPCON2bits.PEN = 1;//GIVE STOP CONDITION
        while(SSPCON2bits.PEN);//WAIT FOR STOP CONDITION TO END
        return;
    }
    //--------------------- NUMBER 1


    SSPBUF = ADDRESSHIGH;//LOAD HIGH ADDRESS
    while(!PIR1bits.SSPIF);//SAME AS 1
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return;
    }
    //SAME AS 1

    SSPBUF = ADDRESSLOW;//LOAD LOW ADDRESS BYTE
    while(!PIR1bits.SSPIF);//SAME AS 1
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return;
    }
    //SAME AS 1

    SSPBUF = EEPROMDATA;//LOAD DATA TO ADDRESSHIGH+ADDRESSLOW
    while(!PIR1bits.SSPIF);//WAIT FOR DATA TO BE SENT
    PIR1bits.SSPIF = 0;//CLEAR INTERRUPT FLAG

    SSPCON2bits.PEN = 1;//GIVE STOP BIT
    while(SSPCON2bits.PEN);//WAIT FO RSTOP CONDITION TO COMPLETE
    //TRANSMISSION OF A BYTE ENDS HERE

}

char EEPROMREAD(char SLAVEADDRESS, char ADDRESSHIGH, char ADDRESSLOW)
{
    char TempData;

    //START
    SSPCON2bits.SEN =1;//SAME AS 1
    while(SSPCON2bits.SEN);
    PIR1bits.SSPIF =0;

    SSPBUF = SLAVEADDRESS;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return(0xFF);
    }
    //SAME AS 1


    SSPBUF = ADDRESSHIGH;//LOAD ADDRESS HIGH
    while(!PIR1bits.SSPIF);//SAME AS 1
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return(0xFF);
    }
    //SAME AS 1

    SSPBUF = ADDRESSLOW;//LOAD ADDRESS LOW
    while(!PIR1bits.SSPIF);//SAME AS 1
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return(0xFF);
    }
    //SAME AS 1

    //SEND A REPEATED START FOR GETTING THE DATA FROM ADDRESSHIGH+ADDRESSLOW
    SSPCON2bits.RSEN = 1;
    while(SSPCON2bits.RSEN);//WAIT FOR REAPETED START TO FINISH
    PIR1bits.SSPIF = 0;

    SSPBUF = (SLAVEADDRESS+1);//SLAVEADDRESS = WRITE AND WRITE == 0, FOR READ, SLAVE ADDRESS+1,SINCE READ == 1
    while(!PIR1bits.SSPIF);//WAIT FOR ADDRESS TO GET WRITTEN
    PIR1bits.SSPIF = 0;//CLEAR THE FLAG
    if(SSPCON2bits.ACKSTAT)//CHECK FOR ACK
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN);
        return(0xFF);
    }

    SSPCON2bits.RCEN = 1;//MAKE RECIEVE ENABLE
    while(!SSPSTATbits.BF);//WAIT FOR SSPBUF TO GET FULL
    TempData = SSPBUF;//READ SSPVUF

    SSPCON2bits.ACKDT = 1;//LOAD ACKNOWLEDGE DATA, NACK = 1
    SSPCON2bits.ACKEN = 1;//TRANSMITT NOT ACKNOWLEDGE
    while(SSPCON2bits.ACKEN);//WAITNG TO SEND NACK

    SSPCON2bits.PEN = 1;//ISSUES STOP CONDITION
    while(SSPCON2bits.PEN);//WAIT FOR STOP CONDITION TO GET COMPLETE
    return TempData;//RETURN WITH THE DATAT READ

}

void LCDINITIALIZE()
{
    LATC = 0x00;
    TRISC = 0xD7;
    ANSELC = 0xC4;
    SPI1_Initialize();
    lcdinit();
}

void TIMERandEXT()
{
   //I3OT3
    //Output
    TRISEbits.RE0 = 0;
    TRISEbits.RE1 = 0;
    PORTEbits.RE0 = 0;
    //Input
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    //Intcon : Global and Local
    INTCONbits.GIE = 1;
    INTCONbits.TMR0IE = 1;
    INTCONbits.INT0IE = 0;//?????
    INTCON3bits.INT1IE = 0;

    //edge
    INTCON2bits.INTEDG0 = 0;
    //Interrupt flag
    INTCONbits.TMR0IF = 0;
    INTCONbits.INT0IF = 0;
    INTCON3bits.INT1IF = 0;
    //Timer load , timer control
    T0CON = 0x07;
    TMR0H = 0XE1;//0X66;
    TMR0L = 0X4D;//0XB4;
    //TMR0H = 0X66;
  //  TMR0L = 0XB4;
}

void ADCINIT()
{
//ADC CONTROL Bits setting
	ANSA0=1;  // select RA0 as analog input
    ADCON2 =0xAD;  // left justified, 12TAC
    ADCON1 =0x00;  //reference voltage vcc,vss
	ADCON0 =0x01;  //RA0 as analog input to ADC
    //ADC ends
    //ADC
    TRISAbits.RA0 = 1;
    TRISAbits.RA1 = 1;
}

char ADC_READING(char INT_SENSOR_ , char EXT_SENSOR_)
{
    ADCON0bits.CHS0 = 0;
    ADCON0bits.GO = 1;
    INT_SENSOR =ADRES;
    INT_SENSOR = 9.45*(INT_SENSOR*0.0047662602)+13;
    INT_SENSOR = (INT_SENSOR*1.8)+32;

   INT_SENSOR_= INT_SENSOR;
    ADCON0bits.GO = 0;
    __delay_ms(750);

    ADCON0bits.CHS0 = 1;
    ADCON0bits.GO = 1;
    EXT_SENSOR =ADRES;
    EXT_SENSOR = 9.2*(EXT_SENSOR*0.0048882114)+12;
    EXT_SENSOR = (EXT_SENSOR*1.8)+32;
  EXT_SENSOR_ =EXT_SENSOR;

    ADCON0bits.GO = 0;
    return INT_SENSOR_,EXT_SENSOR_ ;
}
