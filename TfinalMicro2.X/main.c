
#include "mcc_generated_files/mcc.h"
#include <xc.h>
#include <stdio.h>
#define ValorAscii '0' //constante para sumar a entero y convertir a ASCII

char Arreglo[9]={0};
//unsigned int S1[2]={0};
//unsigned int S2[2]={0};
char LM335[2]={0};
char PT100[2]={0};

void Iniciar_Puertos(){
    TRISB=0x00; //Se establece Puerto B como salida
    TRISC=0x01; //Se establece Puerto RC0 como entrada y resto como salida
    ANSELB=0;   //Se apaga el modo Análogo del puerto B
    //ANSELA = 0x3F; //Se Enciende pines en modo Análogo para Usar ADC en puerto A
    PORTC=0;    //Inicia Puerto C en cero
    PORTB=0;    //Inicia Puerto B en cero
}

 void Iniciar_UART(void)
{
    TRISCbits.TRISC6=0; //Pin C6 como salida de transmisión serial [Pin (TX)]
    TRISCbits.TRISC7=1; //Pin C7 como entrada de transmisión serial [Pin (RX)]
    TXSTAbits.SYNC=0;   //Se establece el modo asíncronono de la EUSART
    TXSTAbits.TX9=0;    //Trama de 8 bits para la transmisión
    TXSTAbits.BRGH=1;   // Comunicación a Alta Velocidad en la UART
    BRG16=1;            // Bandera requerida para alcanzar alta velocidad
    SPBRG=8;            //  Velocidad de Transmision a 115200Baud con 4MHz
    RCSTAbits.SPEN=1;   //Habilitación del Puerto Seríal
    TXSTAbits.TXEN=1;   // Enciende la Transmisión de Datos
    RCSTAbits.CREN=1;   // Recepción de datos ON
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

char RecibirDato_UART(void){
    if(OERR) // check for Error 
    {
        RCSTAbits.CREN = 0; //If error -> Reset 
        RCSTAbits.CREN = 1; //If error -> Reset 
    }
    
    while(!RCIF);  // hold the program till RX buffer is free
    
    return RCREG; //receive the value and send it to main function
}

void Informacion(void){
        Arreglo[0]=RecibirDato_UART(); //   Switch
        Arreglo[1]=RecibirDato_UART(); //Primer Digito  LM335
        Arreglo[2]=RecibirDato_UART(); //Segundo digito LM335
        Arreglo[3]=RecibirDato_UART(); //Primer Digito PT100
        Arreglo[4]=RecibirDato_UART(); //Segundo Digito PT100
        Arreglo[5]=RecibirDato_UART(); // bandera S1 baja
        Arreglo[6]=RecibirDato_UART(); // bandera S2 alta
        Arreglo[7]=RecibirDato_UART(); // bandera S1 alta
        Arreglo[8]=RecibirDato_UART(); // bandera S2 alta
}
 
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    Iniciar_Puertos(); //Función Para configurar Puertos del PIC
    Iniciar_UART();    //Configura el módulo UART

    while (1)
    {
        Informacion();
        if(Arreglo[5]=='1'){ //si se activa ALS1 enciende led
            PORTBbits.RB0=1;
            PORTBbits.RB1=0;
        }
        else if(Arreglo[7]=='1'){//si se activa AHS1 enciende led
            PORTBbits.RB0=0;
            PORTBbits.RB1=1;
        }
        else{ // SI no hay alarmas activas en S1
            PORTBbits.RB0=0;
            PORTBbits.RB1=0;
        }
        
        if(Arreglo[6]=='1'){ //si se activa ALS2 enciende led
            PORTBbits.RB2=1;
            PORTBbits.RB3=0;
        }
        else if(Arreglo[8]=='1'){ //si e activa AHS2 enciende led
            PORTBbits.RB2=0;
            PORTBbits.RB3=1;
        }
        else{ // SI no hay alarmas activas en S2
            PORTBbits.RB2=0;
            PORTBbits.RB3=0;
        }
        
        
        LM335[0]=Arreglo[1]; //Arreglo que contiene los digitos del valor del sensor1
        LM335[1]=Arreglo[2]; 
        PT100[0]=Arreglo[3]; //Arreglo que contiene los digitos del valor del sensor2
        PT100[1]=Arreglo[4];
        
        EnviarTexto_UART("SW:"); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[0]); //Envía a la Terminal Estado del Switch
        EnviarDato_UART('\n'); //Nueva Linea en la Terminal (ENTER)
        EnviarDato_UART('\r'); 
        EnviarTexto_UART("LM:"); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[1]); //Envía a la Terminal valor de sensor1
        EnviarDato_UART(Arreglo[2]);
        EnviarTexto_UART("  PT:"); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[3]);//Envía a la Terminal valor de sensor2
        EnviarDato_UART(Arreglo[4]);
        EnviarDato_UART('\n'); //Nueva Linea en la Terminal (ENTER)
        EnviarDato_UART('\r');
        EnviarTexto_UART("INF1: "); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[5]); //Envía Bandera de alarma 
        EnviarTexto_UART("  SUP2: "); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[6]); //Envía Bandera de alarma
        EnviarDato_UART('\n'); //Nueva Linea en la Terminal (ENTER)
        EnviarDato_UART('\r');
        EnviarTexto_UART("SUP1: "); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[7]); //Envía Bandera de alarma
        EnviarTexto_UART("  SUP2: "); //Envía cadena de Texto
        EnviarDato_UART(Arreglo[8]); //Envía Bandera de alarma
        EnviarDato_UART('\n'); //Nueva Linea en la Terminal (ENTER)
        EnviarDato_UART('\r');    
    }
}
