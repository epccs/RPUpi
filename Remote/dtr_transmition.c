/*
DTR pair transmition for RPUBUS manager
Copyright (C) 2019 Ronald Sutherland

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <util/delay.h>
#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/uart.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "rpubus_manager_state.h"
#include "dtr_transmition.h"

unsigned long uart_started_at;

volatile uint8_t uart_output;

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
            return;
        }
        if (input == RPU_ARDUINO_MODE) 
        {
            arduino_mode_started = 0;
            arduino_mode = 1;
            return;
        }
        if (input == RPU_START_TEST_MODE) 
        {
            // fill transceiver_state with 0:0:TX_nRE:TX_DE:DTR_nRE:DTR_DE:RX_nRE:RX_DE
            transceiver_state =   (digitalRead(TX_nRE)<<5) | (digitalRead(TX_DE)<<4) | (digitalRead(DTR_nRE)<<3) | (digitalRead(DTR_DE)<<2) | (digitalRead(RX_nRE)<<1) | (digitalRead(RX_DE));
            
            // turn off transceiver controls except the DTR recevior
            digitalWrite(TX_nRE, HIGH);
            digitalWrite(TX_DE, LOW);
            // DTR_nRE active would block uart from seeing RPU_END_TEST_MODE
            digitalWrite(DTR_DE, LOW); 
            digitalWrite(RX_nRE, HIGH);
            digitalWrite(RX_DE, LOW);

            test_mode_started = 0;
            test_mode = 1;
            return;
        }
        if (input == RPU_END_TEST_MODE) 
        {
            // recover transceiver controls
            digitalWrite(TX_nRE, ( (transceiver_state>>5) & 0x01) );
            digitalWrite(TX_DE, ( (transceiver_state>>4) & 0x01) );
            // DTR_nRE is always active
            // DTR_DE was made active when the I2C command ran fnEndTestMode()
            digitalWrite(RX_nRE, ( (transceiver_state>>1) & 0x01) );
            digitalWrite(RX_DE, ( (transceiver_state) & 0x01) );

            test_mode_started = 0;
            test_mode = 0;
            return;
        }
        if (input == rpu_address) // that is my local address
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
            return;
        }
        if (input <= 0x7F) // values > 0x80 are for a host disconnect e.g. the bitwise negation of an RPU_ADDRESS
        {  
            lockout_active =1;
            bootloader_started = 0;
            host_active =0;

            connect_lockout_mode();

            lockout_started_at = millis();
            blink_started_at = millis();
            return;
        }
        if (input > 0x7F) // RPU_HOST_DISCONNECT is the bitwise negation of an RPU_ADDRESS it will be > 0x80 (seen as a uint8_t)
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
            return;
        } 
    }
    else if (uart_has_TTL && (kRuntime > UART_TTL) )
    { // perhaps the DTR line is stuck (e.g. someone has pulled it low) so may need to time out
        status_byt &= (1<<DTR_READBACK_TIMEOUT);
        uart_has_TTL = 0;
    }
}