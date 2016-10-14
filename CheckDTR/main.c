/* RPUpi CheckDTR for hardware testing
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
 *
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"

static unsigned long blink_started_at;
static uint8_t Check_Diff_Bias;

void blink(uint8_t count)
{
    unsigned long kRuntime;
    
    for (uint8_t i=0 ;i<count; i++)
    {
        digitalToggle(LED_BUILTIN);
        kRuntime = 0;
        blink_started_at = millis();
        while (kRuntime <= 300) // wait for 1/10 second
        {
            kRuntime = millis() - blink_started_at;
        }
        digitalToggle(LED_BUILTIN);
        kRuntime = 0;
        blink_started_at = millis();
        while (kRuntime <= 300) // wait for 1/10 second
        {
            kRuntime = millis() - blink_started_at;
        }
    }
}

int main(void)
{
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,LOW);

    // setup RTS/CTS so they are disabled
    pinMode(HOST_nRTS, INPUT);
    digitalWrite(HOST_nRTS, HIGH); // with AVR when the pin DDR is set as an input setting pin high will trun on a weak pullup 
    pinMode(HOST_nCTS, OUTPUT);
    digitalWrite(HOST_nCTS, HIGH);

    // setup DTR/DSR, they are just test points on RPUpi
    pinMode(HOST_nDTR, INPUT);
    digitalWrite(HOST_nDTR, HIGH); // another weak pullup 
    pinMode(HOST_nDSR, OUTPUT);
    digitalWrite(HOST_nDSR, LOW);

    // setup DTR transceiver
    pinMode(DTR_TXD,OUTPUT);
    digitalWrite(DTR_TXD,LOW);
    pinMode(DTR_DE,OUTPUT);
    digitalWrite(DTR_DE, LOW);  // yep there is startup glitch, so disallow DTR pair driver until after the UART is running.
    pinMode(DTR_nRE,OUTPUT);
    digitalWrite(DTR_nRE,LOW);

    // setup TX and RX transceiver so both the host and node are locked out from the differential pairs 
    pinMode(RX_DE, OUTPUT);
    digitalWrite(RX_DE, LOW);  // disallow RX pair driver to enable if FTDI_TX is low
    pinMode(RX_nRE, OUTPUT);
    digitalWrite(RX_nRE, HIGH);  // disable RX pair recevior to output to local MCU's RX input
    pinMode(TX_DE, OUTPUT);
    digitalWrite(TX_DE, LOW); // disallow TX pair driver to enable if TX (from MCU) is low
    pinMode(TX_nRE, OUTPUT);
    digitalWrite(TX_nRE, HIGH);  // disable TX pair recevior to output to FTDI_RX input

    // do not use this firmware with a node board, or a Pi Zero 
    pinMode(nSS, OUTPUT); // nSS is connected to a open collector buffer used to pull the node's MCU nRESET low
    digitalWrite(nSS, LOW); 
    pinMode(SHUTDOWN, INPUT);
    digitalWrite(SHUTDOWN, HIGH); // weak pull-up. This is used to remove RX/TX lockout

    initTimers(); //Timer0 Fast PWM mode, Timer1 & Timer2 Phase Correct PWM mode.

    sei(); // Enable global interrupts to start TIMER0

    // the UART has a startup glitch... this should keep it from to the DTR pair.
    // NOTE UART is not initalized for this program, but will keep the logic.
    _delay_ms(10);
    digitalWrite(DTR_DE, HIGH); 

    while (1) 
    {
        _delay_ms(4000);

        // set DTR transceiver DI input with 0V.
        digitalWrite(DTR_TXD,LOW);
        // set DTR transceiver DTR_DE HIGH to drive DTR pair, R12 has 3.3V.
        digitalWrite(DTR_DE,HIGH);
        // set DTR_nRE LOW cause transceiver to copy DTR pair to RO output
        digitalWrite(DTR_nRE,LOW);
        
        // DTR pair should be driven LOW by DI and copy its value to RO
        do
        {    
            blink(1);
            _delay_ms(1000);
        } while (digitalRead(DTR_RXD) != LOW);

        // set DTR transceiver DI input with 3.3V should cause an undriven pair
        digitalWrite(DTR_TXD,HIGH);

        // a High on DTR_TXD causes Q2 to pull down DTR_DE so the
        // DTR pair should be undriven which is seen at RO as a HIGH 
        do
        {    
            blink(2);
            _delay_ms(1000);
        } while(digitalRead(DTR_RXD) != HIGH);
 
        digitalWrite(DTR_DE,LOW);
        // set DTR_nRE HIGH to disconnect transceiver so R10 pulls up RO output
        digitalWrite(DTR_nRE,HIGH);
        digitalWrite(DTR_TXD,LOW);

        // DTR pair should be driven LOW but RO is disconnected and should pull-up to PASS
        do
        {    
            blink(3);
            _delay_ms(1000);
        } while (digitalRead(DTR_RXD) != HIGH);

        // set DTR_DE HIGH to copy DI onto DTR pair.
        digitalWrite(DTR_DE,HIGH);

        // DTR pair should not be driven and RO is disconnected and should pull-up to PASS
        do
        {    
            blink(4);
            _delay_ms(1000);
        } while (digitalRead(DTR_RXD) != HIGH);

        uint8_t shutdown_now = !digitalRead(SHUTDOWN);

        // if the shutdown switch is pressed then remove RX/TX lockout
        if (!Check_Diff_Bias && shutdown_now)
        {
            Check_Diff_Bias = 1;
            digitalWrite(RX_DE, HIGH);  // allow RX pair driver to enable if FTDI_TX is low
            digitalWrite(RX_nRE, LOW);  // enable RX pair recevior to output to local MCU's RX input
            digitalWrite(TX_DE, HIGH); // allow TX pair driver to enable if TX (from MCU) is low
            digitalWrite(TX_nRE, LOW);  // enable TX pair recevior to output to FTDI_RX input
        }
    }    
}

