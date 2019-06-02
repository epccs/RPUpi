/* Remote is a RPUBUS manager firmware
Copyright (C) 2019 Ronald Sutherland

 This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

For a copy of the GNU General Public License use
http://www.gnu.org/licenses/gpl-2.0.html
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "../lib/timers.h"
#include "../lib/twi0.h"
#include "../lib/twi1.h"
#include "../lib/uart.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "rpubus_manager_state.h"
#include "i2c_cmds.h"
#include "smbus_cmds.h"
#include "id_in_ee.h"


void setup(void) 
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(HOST_nRTS, INPUT);
    digitalWrite(HOST_nRTS, HIGH); // with AVR when the pin DDR is set as an input setting pin high will trun on a weak pullup 
    pinMode(HOST_nCTS, OUTPUT);
    digitalWrite(HOST_nCTS, HIGH);
    pinMode(HOST_nDTR, INPUT);
    digitalWrite(HOST_nDTR, HIGH); // another weak pullup 
    pinMode(HOST_nDSR, OUTPUT);
    digitalWrite(HOST_nDSR, HIGH);
    pinMode(RX_DE, OUTPUT);
    digitalWrite(RX_DE, HIGH);  // allow RX pair driver to enable if FTDI_TX is low
    pinMode(RX_nRE, OUTPUT);
    digitalWrite(RX_nRE, LOW);  // enable RX pair recevior to output to local MCU's RX input
    pinMode(TX_DE, OUTPUT);
    digitalWrite(TX_DE, HIGH); // allow TX pair driver to enable if TX (from MCU) is low
    pinMode(TX_nRE, OUTPUT);
    digitalWrite(TX_nRE, LOW);  // enable TX pair recevior to output to FTDI_RX input
    pinMode(DTR_DE, OUTPUT);
    digitalWrite(DTR_DE, LOW);  // seems to be a startup glitch ??? so disallow DTR pair driver to enable if DTR_TXD is low
    pinMode(nSS, OUTPUT); // nSS is input to a Open collector buffer used to pull to MCU nRESET low
    digitalWrite(nSS, HIGH); 
    pinMode(SHUTDOWN, INPUT);
    digitalWrite(SHUTDOWN, HIGH); // trun on a weak pullup 

    bootloader_address = RPU_HOST_CONNECT; 
    host_active = 0;
    lockout_active = 0;
    status_byt = 0;
    write_rpu_address_to_eeprom = 0;
    shutdown_detected = 0;
    shutdown_started = 0;

    //Timer0 Fast PWM mode, Timer1 & Timer2 Phase Correct PWM mode.
    initTimers(); 

    /* Initialize UART, it returns a pointer to FILE so redirect of stdin and stdout works*/
    stdout = stdin = uartstream0_init(BAUD);

    // can use with a true I2C bus master that does clock stretching and repeated starts 
    twi0_setAddress(I2C0_ADDRESS);
    twi0_attachSlaveTxEvent(transmit_i2c_event); // called when I2C slave has been requested to send data
    twi0_attachSlaveRxEvent(receive_i2c_event); // called when I2C slave has received data
    twi0_init(false); // do not use internal pull-up

    // can use with a SMbus bus master that has to deal with constraints imposed by time sharing 
    twi1_setAddress(I2C1_ADDRESS);
    twi1_attachSlaveTxEvent(transmit1_event); // called when I2C1 slave has been requested to send data
    twi1_attachSlaveRxEvent(receive1_event); // called when I2C1 slave has received data
    twi1_init(false); // do not use internal pull-up a Raspberry Pi has them on board

    sei(); // Enable global interrupts to start TIMER0 and UART
    
    _delay_ms(50); // wait for UART glitch to clear
    digitalWrite(DTR_DE, HIGH);  // allow DTR pair driver to enable if DTR_TXD is low

    // Use eeprom value for rpu_address if ID was valid    
    if (check_for_eeprom_id() )
    {
        rpu_address = eeprom_read_byte((uint8_t*)(EE_RPU_ADDRESS));
    }
    else
    {
        rpu_address = RPU_ADDRESS;
    }
    
    // is foreign host in control? (ask over the DTR pair)
    uart_has_TTL = 0;
    
#if defined(DISCONNECT_AT_PWRUP)
    // at power up send a byte on the DTR pair to unlock the bus 
    // problem is if a foreign host has the bus this would be bad
    uart_started_at = millis();
    uart_output= RPU_HOST_DISCONNECT;
    printf("%c", uart_output); 
#endif
#if defined(HOST_LOCKOUT)
// this will keep the host off the bus until the HOST_LOCKOUT_STATUS bit in status_byt is clear 
// status_byt is zero at this point, but this shows how to set the bit without changing other bits
    status_byt |= (1<<HOST_LOCKOUT_STATUS);
#endif
}

// blink if the host is active, fast blink if status_byt, slow blink in lockout
void blink_on_activate(void)
{
    if (shutdown_detected) // do not blink,  power usage needs to be very stable to tell if the host has haulted. 
    {
        return;
    }
    
    unsigned long kRuntime = millis() - blink_started_at;
    
    // Remote will start with the lockout bit set so don't blink for that
    if (!(status_byt & ~(1<<HOST_LOCKOUT_STATUS) )) 
    {
        // blink half as fast when host is foreign
        if (host_is_foreign)
        {
            kRuntime = kRuntime >> 1;
        }
        
        if ( bootloader_started  && (kRuntime > BLINK_BOOTLD_DELAY) )
        {
            digitalToggle(LED_BUILTIN);
            
            // next toggle 
            blink_started_at += BLINK_BOOTLD_DELAY; 
        }
        else if ( lockout_active  && (kRuntime > BLINK_LOCKOUT_DELAY) )
        {
            digitalToggle(LED_BUILTIN);
            
            // next toggle 
            blink_started_at += BLINK_LOCKOUT_DELAY; 
        }
        else if ( host_active  && (kRuntime > BLINK_ACTIVE_DELAY) )
        {
            digitalToggle(LED_BUILTIN);
            
            // next toggle 
            blink_started_at += BLINK_ACTIVE_DELAY; 
        }
        // else spin the loop
    }
    else
    {
        if ( (kRuntime > BLINK_STATUS_DELAY))
        {
            digitalToggle(LED_BUILTIN);
            
            // next toggle 
            blink_started_at += BLINK_STATUS_DELAY; 
        }
    }
}

void check_Bootload_Time(void)
{
    if (bootloader_started)
    {
        unsigned long kRuntime = millis() - bootloader_started_at;
        
        if (!arduino_mode && (kRuntime > BOOTLOADER_ACTIVE))
        {
            connect_normal_mode();
            host_active =1;
            bootloader_started = 0;
        }
    }
}

void check_DTR(void)
{
    if (!host_is_foreign) 
    {
        if ( !digitalRead(HOST_nDTR) || !digitalRead(HOST_nRTS) )  // if HOST_nDTR or HOST_nRTS are set (active low) then assume avrdude wants to use the bootloader
        {
            if ( !(status_byt & (1<<HOST_LOCKOUT_STATUS)) )
            {
                if (digitalRead(HOST_nCTS))
                { // tell the host that it is OK to send
                    digitalWrite(HOST_nCTS, LOW);
                    digitalWrite(HOST_nDSR, LOW);
                }
                else
                {
                    if ( !(bootloader_started  || lockout_active || host_active || uart_has_TTL) )
                    {
                        // send the bootload_addres on the DTR pair when nDTR/nRTS becomes active
                        uart_started_at = millis();
                        uart_output= bootloader_address; // set by I2C, default is RPU_HOST_CONNECT
                        printf("%c", uart_output); 
                        uart_has_TTL = 1;
                        localhost_active = 1;
                    }
                }
            }
        }
        else
        {
            if ( host_active && localhost_active && (!uart_has_TTL) && (!bootloader_started) && (!lockout_active) )
            {
                // send a byte on the DTR pair when FTDI_nDTR is first non-active
                uart_started_at = millis();
                uart_output= RPU_HOST_DISCONNECT;
                printf("%c", uart_output); 
                uart_has_TTL = 1;
                digitalWrite(LED_BUILTIN, HIGH);
                localhost_active = 0;
                digitalWrite(HOST_nCTS, HIGH);
                digitalWrite(HOST_nDSR, HIGH);
            }
        }
    }
}

// lockout needs to happoen for a long enough time to insure bootloading is finished,
void check_lockout(void)
{
    unsigned long kRuntime = millis() - lockout_started_at;
    
    if (!arduino_mode && ( lockout_active && (kRuntime > LOCKOUT_DELAY) ))
    {
        connect_normal_mode();

        host_active = 1;
        lockout_active =0;
    }
}

/* The UART is connected to the DTR pair which is half duplex, 
     but is self enabling when TX is pulled low.
*/
void check_uart(void)
{
    unsigned long kRuntime = millis() - uart_started_at;
 
    if ( uart0_available() && !(uart_has_TTL && (kRuntime > UART_TTL) ) )
    {
        uint8_t input;
        input = (uint8_t)(getchar());

        // was this byte sent with the local DTR pair driver, if so the status_byt may need update
        // and the lockout from a local host needs to be treated differently
        // need to ignore the local host's nRTS if getting control from a remote host
        if ( uart_has_TTL )
        {
            if(input != uart_output) 
            { // sent byte does not match,  but I'm not to sure what would cause this.
                status_byt &= (1<<DTR_READBACK_NOT_MATCH);
            }
            uart_has_TTL = 0;
            host_is_foreign = 0;
        }
        else
        {
            if (localhost_active)
            {
                host_is_foreign = 0; // used to connect the host
            }
            else
            {
                host_is_foreign = 1; // used to lockout the host
            }
        }

        if (input == RPU_NORMAL_MODE) // end the lockout or bootloader if it was set.
        { 
            lockout_started_at = millis() - LOCKOUT_DELAY;
            bootloader_started_at = millis() - BOOTLOADER_ACTIVE;
            digitalWrite(LED_BUILTIN, LOW);
            arduino_mode = 0;
            blink_started_at = millis();
        }
        else if (input == RPU_ARDUINO_MODE) 
        {
            arduino_mode_started = 0;
            arduino_mode = 1;
        }
        else if (input == rpu_address) // that is my local address
        {
            connect_bootload_mode();

            // start the bootloader
            bootloader_started = 1;
            digitalWrite(nSS, LOW);   // nSS goes through a open collector buffer to nRESET
            _delay_ms(20);  // hold reset low for a short time 
            digitalWrite(nSS, HIGH); // this will release the buffer with open colllector on MCU nRESET.
            local_mcu_is_rpu_aware = 0; // after a reset it may be loaded with new software
            blink_started_at = millis();
            bootloader_started_at = millis();
        }
        else if (input <= 0x7F) // values > 0x80 are for a host disconnect e.g. the bitwise negation of an RPU_ADDRESS
        {  
            lockout_active =1;
            bootloader_started = 0;
            host_active =0;

            connect_lockout_mode();

            lockout_started_at = millis();
            blink_started_at = millis();
        }
        else if (input > 0x7F) // RPU_HOST_DISCONNECT is the bitwise negation of an RPU_ADDRESS it will be > 0x80 (seen as a uint8_t)
        { 
            host_is_foreign = 0;
            lockout_active =0;
            host_active =0;
            bootloader_started = 0;
            digitalWrite(LED_BUILTIN, HIGH);
            digitalWrite(RX_DE, LOW); // disallow RX pair driver to enable if FTDI_TX is low
            digitalWrite(RX_nRE, HIGH);  // disable RX pair recevior to output to local MCU's RX input
            digitalWrite(TX_DE, LOW); // disallow TX pair driver to enable if TX (from MCU) is low
            digitalWrite(TX_nRE, HIGH);  // disable TX pair recevior that outputs to FTDI_RX input
        } 
    }
    else if (uart_has_TTL && (kRuntime > UART_TTL) )
    { // perhaps the DTR line is stuck (e.g. someone has pulled it low) so may need to time out
        status_byt &= (1<<DTR_READBACK_TIMEOUT);
        uart_has_TTL = 0;
    }
}



void check_shutdown(void)
{
    if (shutdown_started)
    {
        unsigned long kRuntime = millis() - shutdown_started_at;
        
        if ( kRuntime > SHUTDOWN_TIME)
        {
            pinMode(SHUTDOWN, INPUT);
            digitalWrite(SHUTDOWN, HIGH); // trun on a weak pullup 
            shutdown_started = 0; // set with I2C command 5
            shutdown_detected = 1; // clear when reading with I2C command 4
        }
    }
    else
        if (!shutdown_detected)
        { 
            if( !digitalRead(SHUTDOWN) ) 
            {
                pinMode(LED_BUILTIN, OUTPUT);
                digitalWrite(LED_BUILTIN, HIGH);
                shutdown_detected = 1; // clear when reading with I2C command 4
            }
        }
}

int main(void)
{
    setup();

    blink_started_at = millis();

    while (1) 
    {
        blink_on_activate();
        check_Bootload_Time();
        check_DTR();
        check_lockout();
        check_uart();
        if(write_rpu_address_to_eeprom) save_rpu_addr_state();
        check_shutdown();
    }    
}

