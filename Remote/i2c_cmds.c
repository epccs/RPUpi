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

uint8_t i2c0Buffer[I2C_BUFFER_LENGTH];
uint8_t i2c0BufferLength = 0;

// called when I2C data is received. 
void receive_i2c_event(uint8_t* inBytes, int numBytes) 
{
    // table of pointers to functions that are selected by the i2c cmmand byte
    static void (*pf[GROUP][MGR_CMDS])(uint8_t*, int) = 
    {
        {fnRdMgrAddr, fnWtMgrAddr, fnRdBootldAddr, fnWtBootldAddr, fnRdShtdnDtct, fnWtShtdnDtct, fnRdStatus, fnWtStatus},
        {fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull},
        {fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull},
        {fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull}
    };

    // i2c will echo's back what was sent (plus modifications) with transmit event
    for(uint8_t i = 0; i < numBytes; ++i)
    {
        i2c0Buffer[i] = inBytes[i];    
    }
    i2c0BufferLength = numBytes;

    if(i2c0BufferLength <= 1) return; // not valid, do nothing just echo.

    // mask the group bits (6 and 7) so they are alone then roll those bits to the left so they can be used as an index.
    uint8_t group;
    group = (i2c0Buffer[0] & 0xc0) >> 6;
    // if(group >= GROUP) return; // it can not happen

    // mask the command bits (0..5) so they can be used as an index.
    uint8_t command;
    command = i2c0Buffer[0] & 0x3F;
    if(command >= MGR_CMDS) return; // not valid, do nothing but the echo.

    /* Call the command function and return */
    (* pf[group][command])(i2c0Buffer, numBytes);
    return;	
}

void transmit_i2c_event(void) 
{
    // respond with an echo of the last message sent
    uint8_t return_code = twi0_transmit(i2c0Buffer, i2c0BufferLength);
    if (return_code != 0)
        status_byt &= (1<<DTR_I2C_TRANSMIT_FAIL);
}

// I2C_COMMAND_TO_READ_RPU_ADDRESS
void fnRdMgrAddr(uint8_t* inBytes, int numBytes)
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

// I2C_COMMAND_TO_SET_RPU_ADDRESS
void fnWtMgrAddr(uint8_t* i2cBuffer, int numBytes)
{
    rpu_address = i2cBuffer[1];
    write_rpu_address_to_eeprom = 1;
}

// I2C_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR
void fnRdBootldAddr(uint8_t* i2cBuffer, int numBytes)
{
    // replace data[1] with address sent when HOST_nRTS toggles
    i2cBuffer[1] = bootloader_address;
}

// I2C_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR
void fnWtBootldAddr(uint8_t* i2cBuffer, int numBytes)
{
    // set the byte that is sent when HOST_nRTS toggles
    bootloader_address = i2cBuffer[1];
}

// I2C_COMMAND_TO_READ_SW_SHUTDOWN_DETECTED
void fnRdShtdnDtct(uint8_t* i2cBuffer, int numBytes)
{
    // when ICP1 pin is pulled  down the host (e.g. R-Pi Zero) should be set up to hault
    i2cBuffer[1] = shutdown_detected;
    // reading clears this flag that was set in check_shutdown() but it is up to the I2C master to do somthing about it.
    shutdown_detected = 0;
}

// I2C_COMMAND_TO_SET_SW_FOR_SHUTDOWN
void fnWtShtdnDtct(uint8_t* i2cBuffer, int numBytes)
{
    // pull ICP1 pin low to hault the host (e.g. Pi Zero on RPUpi)
    if (i2cBuffer[1] == 1)
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

// I2C_COMMAND_TO_READ_STATUS
void fnRdStatus(uint8_t* i2cBuffer, int numBytes)
{
    i2cBuffer[1] = status_byt;
}

// I2C_COMMAND_TO_SET_STATUS
void fnWtStatus(uint8_t* i2cBuffer, int numBytes)
{
    status_byt = i2cBuffer[1];
}

/* Dummy function */
void fnNull(uint8_t* inBytes, int numBytes)
{
    return; 
}