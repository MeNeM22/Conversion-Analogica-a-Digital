#include <Arduino.h>
#include <avr/io.h>
#include "define.h"

//Contadores para las funciones
int cta = 0; 
int ctb = 0;
int ctc = 0;
int ctd = 1; //Lo igualo a 1 para que empieze primero la funcion 4 sin tocar ningun pulsador

//Extern es para transferir variables de main a void.
extern uint8_t  PuertoD[]; 
extern uint8_t  PuertoB[];
extern uint8_t  VecPuntoA[];
extern uint8_t  Mil, Cent, Dec, Unid ; 
extern uint8_t  ct ;
extern uint8_t  sum;
extern uint16_t Valor_ADCH;

static inline void initADC0(void)
{
  /* usamos como referencia el valor de AVCC */
  ADMUX |= (1 << REFS0);
  /* ADC clock prescaler 16MHz/128 = 125KHz */
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  /* Habillitamos el ADC */
  ADCSRA |= (1 << ADEN);
 }

typedef enum Funciones_Conversor
{
  Funcion_1,
  Funcion_2,
  Funcion_3,
  Funcion_4
} Mod_funcion_t;
Mod_funcion_t Funcion = Funcion_4;

extern void config_TIMER0(void);
extern uint16_t config_ADC(int canal);
extern void FUNCION_DISPLAY(char NUM_DISPLAY);
extern void NUM_ENTERO(int num);
extern void mux (int Mil,int Cent,int Dec,int Unid);
extern void mux3(int Cent, int Dec ,int Unid);
extern void muxA(int Mil, int Cent, int Dec ,int Unid);
extern void VumDig(uint16_t Valor_ADCH);


void PuntoA(void)//Pulsador 1
{
  if (config_ADC(0) > 210 && config_ADC(0) < 250) 
  {
    Funcion = Funcion_1;
    cta = 1;
  }
  else
  {
    PORTD = 0;
    PORTB = 0;
  }
}

void PuntoB(void)//Pulsador 2
{
  if (config_ADC(0) > 500 && config_ADC(0) < 550) 
  {
    ctb = 1;
    Funcion = Funcion_2;
  }
  else
  {
    PORTD = 0;
    PORTB = 0;
  }
}

void PuntoC(void)//Pulsador 3
{
  if (config_ADC(0) > 300 && config_ADC(0) < 350) 
  {
    ctc = 1;
    Funcion = Funcion_3;
  }
  else
  {
    PORTD = 0;
    PORTB = 0;
  }
}

void PuntoD(void)//Pulsador 4
{
  if (config_ADC(0) > 150 && config_ADC(0) < 200) 
  {
    ctd = 1;
    Funcion = Funcion_4;
  }
  else
  {
    PORTD = 0;
    PORTB = 0;
  }
}

int main (void)
 {
  DDRD = 0b11111101;
  DDRB = 0b00111111;
  
  config_TIMER0();
  initADC0();
  sei();

while(1)
  {
    //Llamo las Funciones
    PuntoA();
    PuntoB();
    PuntoC();
    PuntoD();
    //Maquina de estados para funciones
    switch (Funcion)
    {
    case Funcion_1:
    if(cta == 1)
    {
    Valor_ADCH = config_ADC(1); //Igualo el valor_ADCH para que guarde el dato que entra por PC0
    VumDig(Valor_ADCH);
    }
      break;

    case Funcion_2:
    if(ctb == 1)
    {
    Valor_ADCH = (config_ADC(1)) / 4.0117;
    NUM_ENTERO(Valor_ADCH);
    mux3(Cent, Dec, Unid);
    }
      break;

    case Funcion_3:
    if(ctc == 1)
    {
    NUM_ENTERO(config_ADC(1));
    mux(Mil, Cent, Dec, Unid);
    }
      break;

    case Funcion_4:
    if(ctd == 1)
    {
    Valor_ADCH = (config_ADC(1) / 10.23) * 49.95; //De 0000 a 4995mV
    NUM_ENTERO(Valor_ADCH);
    mux(Mil, Cent, Dec, Unid);
    }
      break;
    } 
  }
}
  