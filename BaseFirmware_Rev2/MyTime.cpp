

#include "MyTime.h"
#include <avr/interrupt.h>

// The constructor should be called when the program begins. It 
//  initializes all of the variables used for tracking time to 0
RTC::RTC(){   
// Initializes the palceholder variables. pretty sure 
//  they dont have to be inputs
    _hours = 0;
    _tens = 0;
    _minutes = 0;
    _seconds = 0;

    // calls the function that configures and sets the interrupt timer
    //timerinit();
}

// This function is called by the Constructor and initializes 
//  the timer interrupt to occur every 1 second 
void RTC::timerinit(){
    //cli();      //  stops interrupts  --- IS THIS NECESSARY??
    TCCR1A = 0; //  set control registers to 0
    TCCR1B = 0;
    TCNT1  = 0; //  initialize counter value to 0
    //  set compare match register for 1Hz 
    OCR1A = 15624;
    //  turn on CTC mode - clear timer on compare
    TCCR1B |= (1 << WGM12);
    //  set the prescalar value to 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // enable the timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei();  // allow interrupts
}

// Function for updating time using an external interrupt timer.
//  When the timer rolls over, this function is called which increments 
//  the seconds tracker. When _seconds reaches 60 the other variables 
//  are updated accordingly
void RTC::updateTime(){
    
    if (_seconds == 59){
        if(_minutes == 9){
            _minutes = 0;
            if(_tens == 5){
                _tens = 0;
                if(_hours == 23){
                    _hours = 0;
                }else{_hours += 1;}
            }else{_tens += 1;}
        }else{_minutes += 1;}
        _seconds = 0; 
    }else{_seconds += 1;}
    
    //          !!!!!!!!!!!!!!!!!!!!!!!!
    // insert ISR for alerting state machine a change
    // in time has been made

}

void RTC::setTime(int hours, int tens, int minutes){
    _seconds = 0;
    _hours = hours;
    _tens = tens;
    _minutes = minutes;
}

// The following functions are specifically for displaying the
//  current value of the time keeping variables
int RTC::hours(){
    return _hours;
}
int RTC::tens(){
    return _tens;
}
int RTC::mins(){
    return _minutes;
}
int RTC::sec(){
    return _seconds;
}
