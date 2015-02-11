	

    #include <avr/io.h>
    #include <avr/delay.h>
    #include <avr/interrupt.h>
    #define F_CPU 2000000;
    //USE -O1 optimization when compiling. Otherwise might produce a 37-38KHz signal instead.
     
    uint8_t mask1= 0;
    uint8_t mask2 = 0;
     
    uint8_t ledmask = 0x00;
    int adcresult = 0;
    char bin[4];
     
    //Timer overflows 7  times per second with the 1024 prescaler.
    ISR(TIMER0_OVF_vect){
            //Read ADC result, divide by 64 to get it down to 4 bits and send over IR.
            ADCSRA = (1<<ADEN | 1<<ADSC | 1<<ADIF);
            while((ADCSRA&(1<<ADSC))!=0){
                    asm("nop");
            }
            adcresult = ADC;
            uint8_t conv = adcresult/64;
            binconv(conv, bin);
            sendWord(bin);
     
           
           
    }
    //Sends 1 data pulse over IR led - 20 bursts,
    void sendPulse(){
            uint8_t i = 0;
            while(i<20){
            PORTD = mask1; 
            _delay_us(23);                 
           
            PORTD = mask2;
            _delay_us(23);
            i++;
            }      
    }
    // 1 bit - 20 bursts, 20 pauses.
    void sendBit(){
            sendPulse();
            sendDelay();
    }
    // 0 bit - 40 pauses
    void sendNull(){
            sendDelay();
            sendDelay();
           
    }
     
    //Pause between two messages if transmitting constantly.
    void sendPause(){
           
            uint8_t i = 0;
            while(i<200){
                    asm("nop");
                    _delay_us(23);
                   
                    asm("nop");
                    _delay_us(23);
                    i++;
            }
    }
     
    //Sends the protocol delay - 20*27 microseconds of pause.
    void sendDelay(){
            uint8_t i = 0;
            while(i<20){
                    asm("nop");
                    _delay_us(23);
                   
                    asm("nop");
                    _delay_us(23);
                    i++;
            }      
    }
     
    //Sends the 4-bit data over infrared LED to KITTBOARD
    void sendWord(char* binstr){
            sendBit();
            for(int i=0;i<4;i++){
                    if(binstr[i]=='1'){
                            sendBit();
                    } else {
                            sendNull();
                    }
            }
            sendPause();
    }
     
    //Converts the given integer value into a char array.
    void binconv(int val, char* bin)
    {
        int i;
            //Line terminator
        bin[4] = '\0';
        for (i = 3; i >= 0; --i, val >>= 1)
        {
            bin[i] = (val & 1) + '0';
        }
    }
     
     
    int main(void)
    {
            //LEDs for debugging.
            DDRA = 0xFF;
           
            cli();
            //IR Led init
            mask1 = PORTD | (1<<PD5);
            mask2 = PORTD & ~(1<<PD5);
            DDRD = (DDRD | (1<<PD5));
           
            //Tim0 init.
             TCCR0B = (1<<CS00 | 1<<CS02);  //Timer with 1024 prescaler, we want it to interrupt 7 times per second.
             TIMSK0 = (1<<TOIE0);   //Timer overflow interrupt.
            sei();
            //ADC Init
            ADMUX = (1 << REFS0 | 1 << MUX1);
            ADCSRA = (1 << ADEN | 1 << ADSC);
           
        while(1)
        {
                    asm("nop"); //Let the code stall, wait for interrupts.
            }
    }

