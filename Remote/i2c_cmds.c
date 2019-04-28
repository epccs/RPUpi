/*
i2c_cmds is a i2c RPUBUS manager commands library in the form of a function pointer array  
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

#include <avr/io.h>
#include "../lib/timers.h"
#include "../lib/twi0.h"
#include "../lib/uart.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "rpubus_manager_state.h"
#include "i2c_cmds.h"

uint8_t i2c0Buffer[TWI0_BUFFER_LENGTH];
uint8_t i2c0BufferLength = 0;

// called when I2C data is received. 
void receive_i2c_event(uint8_t* inBytes, int numBytes) 
{
   
    // This buffer will echo's back with transmit0_event()
    for(uint8_t i = 0; i < numBytes; ++i)
    {
        i2c0Buffer[i] = inBytes[i];    
    }
    i2c0BufferLength = numBytes;
    if (i2c0BufferLength > 1)
    {
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_READ_RPU_ADDRESS) )
        {
            i2c0Buffer[1] = rpu_address; // '1' is 0x31
            local_mcu_is_rpu_aware =1;
            
            // end the local mcu lockout. 
            if (localhost_active) 
            {
                // If the local host is active then broadcast on DTR pair
                uart_started_at = millis();
                uart_output = RPU_NORMAL_MODE;
                printf("%c", uart_output); 
                uart_has_TTL = 1; // causes host_is_foreign to be false
            }
            else 
                if (bootloader_started)
                {
                    // If the bootloader_started has not timed out yet broadcast on DTR pair
                    uart_started_at = millis();
                    uart_output = RPU_NORMAL_MODE;
                    printf("%c", uart_output); 
                    uart_has_TTL = 0; // causes host_is_foreign to be true, so local DTR/RTS is not accepted
                } 
                else
                {
                    lockout_started_at = millis() - LOCKOUT_DELAY;
                    bootloader_started_at = millis() - BOOTLOADER_ACTIVE;
                }
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_SET_RPU_ADDRESS) )
        {
            rpu_address = i2c0Buffer[1];
            write_rpu_address_to_eeprom = 1;
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR) )
        {  // read byte sent when HOST_nDTR toggles
            i2c0Buffer[1] = bootloader_address;
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR) ) 
        { // set the byte that is sent when HOST_nDTR toggles
            bootloader_address = i2c0Buffer[1];
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_READ_SW_SHUTDOWN_DETECTED) ) 
        { // when ICP1 pin is pulled  down the host (e.g. Pi Zero on RPUpi) should hault
            i2c0Buffer[1] = shutdown_detected;
             // reading clears this flag that was set in check_shutdown() but it is up to the I2C master to do somthing about it.
            shutdown_detected = 0;
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_SET_SW_FOR_SHUTDOWN) ) 
        { // pull ICP1 pin low to hault the host (e.g. Pi Zero on RPUpi)
            if (i2c0Buffer[1] == 1)
            {
                pinMode(SHUTDOWN, OUTPUT);
                digitalWrite(SHUTDOWN, LOW);
                pinMode(LED_BUILTIN, OUTPUT);
                digitalWrite(LED_BUILTIN, HIGH);
                shutdown_started = 1; // it is cleared in check_shutdown()
                shutdown_detected = 0; // it is set in check_shutdown()
                shutdown_started_at = millis();
            }
            // else ignore
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_READ_STATUS) )
        {
            i2c0Buffer[1] = status_byt;
        }
        if ( (i2c0Buffer[0] == I2C_COMMAND_TO_SET_STATUS) )
        {
            status_byt = i2c0Buffer[1];
        }
    }
}

void transmit_i2c_event(void) 
{
    // respond with an echo of the last message sent
    uint8_t return_code = twi0_transmit(i2c0Buffer, i2c0BufferLength);
    if (return_code != 0)
        status_byt &= (1<<DTR_I2C_TRANSMIT_FAIL);
}