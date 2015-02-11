/* 
 * projekt_KR.c 
 * 
 * Created: 30.05.2014 11:54:08 
 *  Author: KOL 
 */ 
  
#define F_CPU 2000000 
#define PULSIPIKKUS 1111 
#define PULSIDELAY 111 
  
#include <util/delay.h> 
#include <avr/io.h> 
  
void PWM_init(); 
  
//testimiseks 
//void ADC_init(); 
//uint16_t ADC_get(); 
  
  
int main(void) 
{ 
    uint8_t data;   //infrapuna sisendandmed 
    uint8_t aeglusti = 0;   //aeglustab PWM heleduse muutumist 
    uint8_t kiirus;         //PWM heleduse muutumise kiirus 
    uint8_t PWMdata; 
    uint8_t PWMdata_vastand; 
    uint8_t LEDID;              //hetkel põlevate LEDide andmemassiiv. 
    int8_t hetkenihe = 0;       //kuhu hetkel on vaja põlevad LEDid seadistada. 
    uint8_t SUUND_VASAK = 1;    //kas liigutada LEDe vasakule või mitte(paremale) 
    //uint16_t ADC_sisend; 
      
    DDRA = 0xFF; 
    DDRE &= ~(1 << PE5); 
      
    OCR1A = 0; 
    PWM_init(); 
      
    //testimiseks 
    //ADC_init(); 
    kiirus = 1; 
      
    while(bit_is_set(PINE,PE5)); 
      
    while(1) 
    { 
        data = 0; 
        if(bit_is_clear(PINE,PE5))  //check if data transmission has started 
        { 
            _delay_us(PULSIDELAY);          //to reduce the impact of skew 
              
            _delay_us(PULSIPIKKUS);         //read bit 1, MSB of data 
            if(bit_is_clear(PINE,PE5)) data += 8; 
              
            _delay_us(PULSIPIKKUS);         //read bit 2 
            if(bit_is_clear(PINE,PE5)) data += 4; 
              
            _delay_us(PULSIPIKKUS);         //read bit 3 
            if(bit_is_clear(PINE,PE5)) data += 2; 
              
            _delay_us(PULSIPIKKUS);         //read bit 4, LSB of data 
            if(bit_is_clear(PINE,PE5)) data += 1; 
              
            _delay_us(PULSIPIKKUS); 
              
            kiirus = data+1; 
            //PORTA = data; 
        }    
        //testimiseks 
        //ADC_sisend = ADC_get(); 
        //kiirus =  ((ADC_sisend & 0x03C0) >> 6)+1; 
          
          
        aeglusti += 1; 
        if(aeglusti & (1 << 3)) //aeglustab PWM ledi heleduse muutumise kiirust 
        { 
            OCR1A += kiirus; 
            aeglusti = 0; 
        } 
          
        if(OCR1A>1000) 
        { 
            OCR1A = 0; 
            if(SUUND_VASAK) hetkenihe +=1; 
            else hetkenihe -=1; 
            if(hetkenihe > 4) SUUND_VASAK = 0;   //muudab ledide liikumise suunda 
            if(hetkenihe < -5) SUUND_VASAK = 1; 
        } 
          
        LEDID = 0; 
        PWMdata = (PINB & (1 << PB5));    //kopeerib pwm-ledi hetkeoleku tavaledide peale 
        PWMdata_vastand = (~PWMdata & 32); 
        if(SUUND_VASAK)     //kui ledid liiguvad vasakule 
        { 
            if(hetkenihe>0) LEDID = ((PWMdata_vastand >> 2) | 16 | PWMdata) << hetkenihe; 
            else LEDID = ((PWMdata_vastand >> 2) | 16 | PWMdata) >> -hetkenihe; 
        }  
        else
        { 
            if(hetkenihe>0) LEDID = ((PWMdata >> 2) | 16 | PWMdata_vastand) << hetkenihe; 
            else LEDID = ((PWMdata >> 2) | 16 | PWMdata_vastand) >> -hetkenihe; 
        }  
        PORTA = LEDID;  //paneb LEDid põlema õigesti 
    } 
} 
  
void PWM_init() 
{ 
    TCCR1A = (1<<COM1A1|1<<WGM10|1<<WGM11);   //Clear on compare match, 10-bit PWM 
    TCCR1B = (1<<WGM12|1<<CS10);    //10bit PWM, clk/1 prescaler 
    DDRB = 1<<PB5;    //attach LED to PWM 
} 
  
//testimiseks 
/* 
void ADC_init() 
{ 
    ADCSRA |= (1<<ADEN|1<<ADPS0|1<<ADPS2);    //Enables ADC, sets division factor to 32 
    ADMUX |= (1<<REFS0|1<<MUX1);            //Selects ADC2 as input and external voltage as reference point 
} 
//testimiseks 
uint16_t ADC_get() 
{ 
    ADCSRA |= (1<<ADSC);  //Start ADC conversion 
      
    while(1) 
    { 
        if((ADCSRA&(1<<ADSC))!=(1<<ADSC)) 
        { 
            break; 
        } 
    } 
    uint16_t ADC_data; 
    ADC_data = ADC; 
    return ADC_data; 
} 
*/