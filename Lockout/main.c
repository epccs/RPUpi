/* RPUpi Lockout
 * Copyright (C) 2016 Ronald Sutherland
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with the Arduino DigitalIO Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"

#define BLINK_DELAY_OFF 900UL
#define BLINK_DELAY_ON 100UL

static unsigned long blink_at;
volatile uint8_t shutdown_status;

// non-blocking blink (e.g.  _delay_ms(1000) is blocking delay)
void blink_on_active(void)
{
    unsigned long kRuntime = millis() - blink_at;
    
    if ((!shutdown_status) && digitalRead(LED_BUILTIN) && (kRuntime > BLINK_DELAY_OFF) ) 
    {
        digitalWrite(LED_BUILTIN, LOW);
        blink_at += BLINK_DELAY_OFF;
    }
    else if ((!shutdown_status) && (!digitalRead(LED_BUILTIN)) && (kRuntime > BLINK_DELAY_ON) ) 
    {
        digitalWrite(LED_BUILTIN, HIGH);
        blink_at += BLINK_DELAY_ON;
    }
}

int main(void)
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    // setup RTS/CTS so the host can send
    pinMode(HOST_nRTS, INPUT);
    digitalWrite(HOST_nRTS, HIGH); // with AVR when the pin DDR is set as an input setting pin high will trun on a weak pullup 
    pinMode(HOST_nCTS, OUTPUT);
    digitalWrite(HOST_nCTS, LOW);
    
    // setup DTR/DSR, they are just test points on RPUpi
    pinMode(HOST_nDTR, INPUT);
    digitalWrite(HOST_nDTR, HIGH); // another weak pullup 
    pinMode(HOST_nDSR, OUTPUT);
    digitalWrite(HOST_nDSR, LOW);

    // setup DTR transceiver
    pinMode(DTR_TXD, OUTPUT);
    digitalWrite(DTR_TXD, LOW);
    pinMode(DTR_DE, OUTPUT);
    digitalWrite(DTR_DE, LOW);  // yep there is startup glitch, so disallow DTR pair driver until after the UART is running.
    pinMode(DTR_nRE, OUTPUT);
    digitalWrite(DTR_nRE, LOW);

    // setup TX and RX transceiver so both the host and node are locked out from the differential pairs 
    pinMode(RX_DE, OUTPUT);
    digitalWrite(RX_DE, LOW);  // disallow RX pair driver to enable if FTDI_TX is low
    pinMode(RX_nRE, OUTPUT);
    digitalWrite(RX_nRE, HIGH);  // disable RX pair recevior to output to local MCU's RX input
    pinMode(TX_DE, OUTPUT);
    digitalWrite(TX_DE, LOW); // disallow TX pair driver to enable if TX (from MCU) is low
    pinMode(TX_nRE, OUTPUT);
    digitalWrite(TX_nRE, HIGH);  // disable TX pair recevior to output to FTDI_RX input

    // this firmware allows the Pi Zero Serial to be used as a command line interface (e.g. its default setup).
    pinMode(nSS, OUTPUT); // nSS is connected to a open collector buffer that can pull the node's MCU nRESET low
    digitalWrite(nSS, LOW); // hold the node in RESET
    pinMode(SHUTDOWN, INPUT);
    digitalWrite(SHUTDOWN, HIGH); // with AVR when the pin DDR is set as an input setting pin high will trun on a weak pullup 
    
    shutdown_status = 0;
    
    initTimers(); //Timer0 Fast PWM mode, Timer1 & Timer2 Phase Correct PWM mode.

    sei(); // Enable global interrupts to start TIMER0

    // the UART has a startup glitch... this should keep it from to the DTR pair.
    // NOTE UART is not initalized for this program, but will keep the logic.
    _delay_ms(10);
    digitalWrite(DTR_DE, HIGH); 

    while (1) 
    {
        blink_on_active();
        
        uint8_t shutdown_now = ! digitalRead(SHUTDOWN);
        
        if (!shutdown_status && shutdown_now)
        {
            shutdown_status = 1;
            digitalWrite(LED_BUILTIN, HIGH);
        }
    }    
}

