/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.78
        Device            :  PIC16F1937
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/
//Biblioteca de funciones útiles de C
#include "mcc_generated_files/mcc.h"
#include <xc.h>
#include <stdio.h> 
#include <string.h>
#include <math.h>

//Declaración de Variables
unsigned int estadoSwitch=0;
char Arreglo[9]="000000000";
unsigned int AlarmaS1[2]={0,0};
unsigned int AlarmaS2[2]={0,0};
unsigned int ValorADC[2]={0,0};
float LM335=0,PT100=0;

//PINES DEL LCD
#define RS RD2 //Establece RD2 como pin RS del LCD
#define EN RD3 //Establece RD3 como pin Enable del LCD
//La Biblioteca LCD.H Utiliza comunicación a 4 bits (4 MSB)
#define D4 RD4 //Establece RD4 como pin De datos 
#define D5 RD5 //Establece RD5 como pin De datos 
#define D6 RD6 //Establece RD6 como pin De datos 
#define D7 RD7 //Establece RD7 como pin De datos 
#define ValorAscii '0' //Constante para convertir enteros --> Caracter
#define Alarma_bajaS1 AlarmaS1[0]=1;AlarmaS1[1]=0;
#define Alarma_altaS1 AlarmaS1[0]=0;AlarmaS1[1]=1;
#define Alarma_normalS1 AlarmaS1[0]=0;AlarmaS1[1]=0;
#define Alarma_bajaS2 AlarmaS2[0]=1;AlarmaS2[1]=0;
#define Alarma_altaS2 AlarmaS2[0]=0;AlarmaS2[1]=1;
#define Alarma_normalS2 AlarmaS2[0]=0;AlarmaS2[1]=0;
#include "lcd.h" //librería para El manelo de Display LCD 

void Iniciar_Puertos(){
    TRISD=0x00; //Se establece Puerto D como salida
    TRISC=0x01; //Se establece Puerto RC0 como entrada y resto como salida
    TRISA=0xFF; //Se establece Puerto A como salida
    ANSELD=0;   //Se apaga el modo Análogo del puerto D
    ANSELA = 0x3F; //Se Enciende pines en modo Análogo para Usar ADC en puerto A
    PORTC=0;    //Inicia Puerto B en cero
    PORTD=0;    //Inicia Puerto D en cero
}
 void Lcd_float( float valorf){ 
     char s[2];
     sprintf(s, "%.0f", valorf);    
     Lcd_Write_String(s);
  }

void Iniciar_UART(void)
{
    TRISCbits.TRISC6=0; //Pin C6 como salida de transmisión serial [Pin (TX)]
    TXSTAbits.SYNC=0;   //Se establece el modo asíncronono de la EUSART
    TXSTAbits.TX9=0;    //Trama de 8 bits para la transmisión
    TXSTAbits.BRGH=1;   // Comunicación a Alta Velocidad en la UART
    BRG16=1;            // Bandera requerida para alcanzar alta velocidad
    SPBRG=8;            //  Velocidad de Transmision a 115200Baud con 4MHz
    RCSTAbits.SPEN=1;   //Habilitcón del Puerto Seríal
    TXSTAbits.TXEN=1;   // Enciende la Transmisión de Datos
}

void EnviarDato_UART(char DT)  
{
    while(!TXIF);  // Espera que el buffer de transmisión se Libere
    TXREG = DT;    // Carga en el Buffer de transmisión el valor a enviar
}
void EnviarTexto_UART(char* st_pt)
{
    while(*st_pt) //Recorre cadena de texto
        EnviarDato_UART(*st_pt++); //Envía cada caracter de la cadena
}

void Imprimir_LCD(void){
    Lcd_Set_Cursor(1,1);        //Ubica el cursor del LCD en la línea 3
    Lcd_Write_String("Switch:");    //Imprime texto en el LCD
    Lcd_Set_Cursor(2,1);        //Ubica el cursor del LCD en la línea 1
    Lcd_Write_String("LM:");    //Imprime texto en el LCD
    Lcd_float(LM335); //Imprime temperatura obtenida en el LCD
    Lcd_Set_Cursor(2,8);        //Ubica el cursor del LCD en la columna 6
    Lcd_Write_String("PT:");    //Imprime texto en el LCD
    Lcd_float(PT100); //Imprime temperatura obtenida (sensor2)en el LCD
    Lcd_Set_Cursor(3,1);        //Ubica el cursor del LCD 
    Lcd_Write_String("INF1:");    //Imprime texto en el LCD
    Lcd_float(AlarmaS1[0]); 
    Lcd_Set_Cursor(3,9);        //Ubica el cursor del LCD en la en la línea 3
    Lcd_Write_String("SUP1:");    //Imprime texto en el LCD
    Lcd_float(AlarmaS1[1]);  
    Lcd_Set_Cursor(4,1);        //Ubica el cursor del LCD en la línea 3
    Lcd_Write_String("INF2:");    //Imprime texto en el LCD
    Lcd_float(AlarmaS2[0]); 
    Lcd_Set_Cursor(4,9);        //Ubica el cursor del LCD en la columna 6
    Lcd_Write_String("SUP2:");    //Imprime texto en el LCD
    Lcd_float(AlarmaS2[1]); 
}

void informacion(void){ //Prepara en un arreglo los datos a Transmitir
    Arreglo[0]=estadoSwitch+ValorAscii;  //Estado del Switch
    Arreglo[1]=((int)LM335/10)+ValorAscii;//Primer dígito del Sensor1(decenas)
    Arreglo[2]=((int)LM335%10)+ValorAscii;//segundo dígito del Sensor1(unidades)
    Arreglo[3]=((int)PT100/10)+ValorAscii;//Primer dígito del Sensor1(decenas)
    Arreglo[4]=((int)PT100%10)+ValorAscii;  //segundo dígito del Sensor1(unidades)
    Arreglo[5]=AlarmaS1[0]+ValorAscii;   //Bandera para ALS1
    Arreglo[6]=AlarmaS2[0]+ValorAscii;   //Bandera para ALS2
    Arreglo[7]=AlarmaS1[1]+ValorAscii;   //Bandera para AHS1
    Arreglo[8]=AlarmaS2[1]+ValorAscii;   //Bandera para AHS2
}

void ComprobarALarmaS1(void){
        if(LM335<20){ //Activa la alarma baja sel sensor1
            Alarma_bajaS1
        }
        else if(LM335>50){//Activa la alarma alta sel sensor1
            Alarma_altaS1
        }
        else{
            Alarma_normalS1
        }
}
void ComprobarALarmaS2(void){
        if(PT100<30){//Activa la alarma baja sel sensor2
            Alarma_bajaS2
        }
        else if(PT100>60){//Activa la alarma alta sel sensor2
            Alarma_altaS2
        }
        else{
            Alarma_normalS2
        }
}

void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    Iniciar_Puertos(); //Función Para configurar Puertos del PIC
    Lcd_Init();        //Inicia el Display LCD
    Lcd_Clear();       // Borra la pantalla LCD
    ADC_Initialize();  //Configura el módulo ADC
    Iniciar_UART();    //Configura el módulo UART
    
    while (1)
    {
        Imprimir_LCD();
        estadoSwitch=PORTCbits.RC0; //Lee el estado del switch start
        
        switch(estadoSwitch){ // evalúa el estado del switch
            case 0: //Condición de switch cerrado
                Lcd_Set_Cursor(1,8);//Se indica el estado del switch en el LCD
                Lcd_Write_String("OFF");
                break;
            case 1:
                Lcd_Set_Cursor(1,8);//Se indica el estado del switch en el LCD
                Lcd_Write_String("ON ");  
                ADC_SelectChannel(channel_AN0); //selecciona canal ADC para Leer
                ValorADC[0]=ADC_GetConversion(channel_AN0);//Se obtiene la lectura para sensor1
                ADC_SelectChannel(channel_AN1);//selecciona canal ADC para Leer
                ValorADC[1]=ADC_GetConversion(channel_AN1);//Se obtiene la lectura para sensor2
                LM335=((ValorADC[0]*0.489)-273); //Se convierte la lectura ADC a Temperatura(LM335)
                PT100=( 0.526*ValorADC[1]-305.263);//Se convierte la lectura ADC a Temperatura(PT100)
                __delay_us(1000); 
                break;                
        }     
        ComprobarALarmaS1(); //Gestiona el estado de Alarma del sensor 1
        ComprobarALarmaS2(); //Gestiona el estado de Alarma del sensor 1
        informacion();       //Prepara los datos a Transmitir
        EnviarTexto_UART(Arreglo); //Transmite los datos por línea serial
    }   
}
