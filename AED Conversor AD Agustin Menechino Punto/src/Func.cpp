#include <Arduino.h>
#include <avr/io.h>
#include "define.h"

uint8_t PuertoD[]={0b00101000,0b11101000,0b00110000,0b10100000,0b11100000,0b10100000,0b00100000,0b11101000,0b00100000,0b10100000};
uint8_t PuertoB[]={0b00000000,0b00000011,0b00000001,0b00000001,0b00000010,0b00100000,0b00100000,0b00000001,0b00000000,0b00000000};
               //      0           1         2           3          4         5           6          7          8          9      
               
uint8_t VecPuntoA[] = {0b11111000, 0b01111000, 0b01101000};

uint8_t  Mil, Cent, Dec, Unid;
uint8_t  ct  = 0;
uint8_t  sum = 0;
uint16_t Valor_ADCH;


void config_TIMER0(void)
{
    TCCR0A = (1 << WGM01);              //Activa el bit CTC (clear timer on compare match)
                                        //del TCCR0A (timer counter/control register)
    OCR0A = 155;                        //valor de comparacion de int cada 10ms
    TCCR0B = (1 << CS00) | (1 << CS01); 
    TIMSK0 = (1 << OCIE0A);             //Habilita las interrupciones en timer compare
}

static inline void initADC0(void)
{
  /* usamos como referencia el valor de AVCC */
  ADMUX |= (1 << REFS0);
  /* ADC clock prescaler 16MHz/128 = 125KHz */
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
  /* Habillitamos el ADC */
  ADCSRA |= (1 << ADEN);
 }

uint16_t config_ADC(int canal)
{
 ADMUX  &=~ 0x0F;  //seleccion del canal del ADC//
 ADMUX  |= canal;
 ADCSRA |= (1 << ADEN);
 ADCSRA |= (1 << ADSC);

 while(!(ADCSRA & (1 << ADIF)));
 return ADC;
}


void FUNCION_DISPLAY(char NUM_DISPLAY) //Funcion para prender los diplays 1, 2, 3 y 4.
{
  if (NUM_DISPLAY == 1) //Utilizo un if para preguntar si NUM_DISPLAY vale 1
  {
    PRENDER_DISPLAY_1; //si es verdadero prende el dIsplay 1
  }

  if (NUM_DISPLAY == 2)
  {
    PRENDER_DISPLAY_2;
  }

  if (NUM_DISPLAY == 3)
  {
    PRENDER_DISPLAY_3;
  }

  if (NUM_DISPLAY == 4)
  {
    PRENDER_DISPLAY_4;
  }
}

//Funcion para descomponer un numero de 4 digitos
void NUM_ENTERO(int num)
{
  Mil  = num / 1000;
  Cent = (num - (Mil * 1000)) / 100;
  Dec  = (num - (Mil * 1000 + Cent * 100)) / 10;
  Unid = (num - (Mil * 1000 + Cent * 100 + Dec * 10));
}

typedef enum mux //Definicion de tipo de una variable
{                         
  Disp1,          
  Disp2,
  Disp3,
  Disp4}Numero_Display;

//Multiplexado para 4 displays
void mux (int Mil, int Cent, int Dec ,int Unid) 
{
  switch (ct)
  {
  case Disp1:
    PORTD = PuertoD[Mil];
    PORTB = PuertoB[Mil];
    FUNCION_DISPLAY(1);
    break;

  case Disp2:
    PORTD = PuertoD[Cent];
    PORTB = PuertoB[Cent];
    FUNCION_DISPLAY(2);
    break;

  case Disp3:
    PORTD = PuertoD[Dec];
    PORTB = PuertoB[Dec];
    FUNCION_DISPLAY(3);
    break;

  case Disp4:
    PORTD = PuertoD[Unid];
    PORTB = PuertoB[Unid];
    FUNCION_DISPLAY(4);
    break;

  default:
    ct = 0;
    break;
  }
}

//Multiplexado para 3 displays
void mux3(int Cent, int Dec ,int Unid) 
{
  switch (ct)
  {
  case Disp2:
    PORTD = PuertoD[Cent];
    PORTB = PuertoB[Cent];
    FUNCION_DISPLAY(2);
    break;

  case Disp3:
    PORTD = PuertoD[Dec];
    PORTB = PuertoB[Dec];
    FUNCION_DISPLAY(3);
    break;

  case Disp4:
    PORTD = PuertoD[Unid];
    PORTB = PuertoB[Unid];
    FUNCION_DISPLAY(4);
    break;

  default:
    ct = 0;
    break;
  }
}

//Multiplexado para punto A
void muxA(int Mil, int Cent, int Dec ,int Unid)
{
  switch (ct)
  {
  case Disp1:
    PORTD = VecPuntoA[Mil];
    PORTB = 0b00100011;
    FUNCION_DISPLAY(1);
    break;

  case Disp2:
    PORTD = VecPuntoA[Cent];
    PORTB = 0b00100011;
    FUNCION_DISPLAY(2);
    break;

  case Disp3:
    PORTD = VecPuntoA[Dec];
    PORTB = 0b00100011;
    FUNCION_DISPLAY(3);
    break;

  case Disp4:
    PORTD = VecPuntoA[Unid];
    PORTB = 0b00100011;
    FUNCION_DISPLAY(4);
    break;

  default:
    ct = 0;
    break;
  }
}

void VumDig(uint16_t Valor_ADCH)
{
  uint16_t valor = Valor_ADCH;
 if (valor > 0)
 {
   muxA(0, 0, 0, 0);
 }
 if (valor > 50)
 {
   muxA(1, 0, 0, 0);
 }
 if (valor > 255)
 {
   muxA(2, 0, 0, 0);
 }
 if (valor > 383)
 {
   muxA(2, 1, 0, 0);
 }
 if (valor > 511)
 {
   muxA(2, 2, 0, 0);
 }
 if (valor > 649)
 {
   muxA(2, 2, 1, 0);
 }
 if (valor > 767)
 {
   muxA(2, 2, 2, 0);
 }
 if (valor > 895)
 {
   muxA(2, 2, 2, 1);
 }
 if (valor > 1022)
 {
   muxA(2, 2, 2, 2);
 }
}

ISR(TIMER0_COMPA_vect)
{
 sum++;
 if (sum > 5)
      {
        sum = 0;
        ct++;
      }
}