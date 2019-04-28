/*
smbus_cmds is a i2c RPUBUS manager commands library in the form of a function pointer array  
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
#include "../lib/twi1.h"
#include "../lib/uart.h"
#include "../lib/pin_num.h"
#include "../lib/pins_board.h"
#include "rpubus_manager_state.h"
#include "smbus_cmds.h"

uint8_t i2c1Buffer[TWI1_BUFFER_LENGTH];
uint8_t i2c1BufferLength = 0;
uint8_t i2c1_oldBuffer[TWI1_BUFFER_LENGTH]; //i2c1_old is for SMBus
uint8_t i2c1_oldBufferLength = 0;

// called when SMBus slave has received data
void receive1_event(uint8_t* inBytes, int numBytes) 
{
    for(uint8_t i = 0; i < i2c1BufferLength; ++i)
    {
        i2c1_oldBuffer[i] = i2c1Buffer[i];    
    }
    i2c1_oldBufferLength = i2c1BufferLength;
    for(uint8_t i = 0; i < numBytes; ++i)
    {
        i2c1Buffer[i] = inBytes[i];    
    }
    i2c1BufferLength = numBytes;
    // skip commands without data and assume they are for read_i2c_block_data
    if (i2c1BufferLength > 1)
    {
        if ( (i2c1Buffer[0] == SMBUS_COMMAND_TO_READ_RPU_ADDRESS) ) // 0
        {
            i2c1Buffer[1] = rpu_address; // '1' is 0x31
            // host reading does not mean the local_mcu_is_rpu_aware
            // also do not change the bus state
            // just tell the host what the local RPU address is
        }
        if ( (i2c1Buffer[0] == SMBUS_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR) ) // 2
        {  // read byte sent when HOST_nDTR toggles
            i2c1Buffer[1] = bootloader_address;
        }
        if ( (i2c1Buffer[0] == SMBUS_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR) )  // 3
        { // set the byte that is sent when HOST_nDTR toggles
            bootloader_address = i2c1Buffer[1];
        }
        if ( (i2c1Buffer[0] == SMBUS_COMMAND_TO_READ_STATUS) ) // 6
        {
            i2c1Buffer[1] = status_byt;
        }
        if ( (i2c1Buffer[0] == SMBUS_COMMAND_TO_SET_STATUS) ) // 7
        {
            status_byt = i2c1Buffer[1];
        }
    }
}

// called when SMBus slave has been requested to send data
void transmit1_event(void) 
{
    // For SMBus echo the old data from the previous I2C receive event
    twi1_transmit(i2c1_oldBuffer, i2c1_oldBufferLength);
}