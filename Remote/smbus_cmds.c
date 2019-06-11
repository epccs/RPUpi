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
#include "i2c_cmds.h"
#include "smbus_cmds.h"

uint8_t smbusBuffer[SMBUS_BUFFER_LENGTH];
uint8_t smbusBufferLength = 0;
uint8_t smbus_oldBuffer[SMBUS_BUFFER_LENGTH]; //i2c1_old is for SMBus
uint8_t smbus_oldBufferLength = 0;

// called when SMBus slave has received data
void receive1_event(uint8_t* inBytes, int numBytes) 
{
    // table of pointers to functions that are selected by the i2c cmmand byte
    static void (*pf[GROUP][MGR_CMDS])(uint8_t*) = 
    {
        {fnRdMgrAddrQuietly, fnWtMgrAddr, fnRdBootldAddr, fnWtBootldAddr, fnRdShtdnDtct, fnWtShtdnDtct, fnRdStatus, fnWtStatus},
        {fnWtArduinMode, fnRdArduinMode, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull},
        {fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull},
        {fnStartTestMode, fnEndTestMode, fnNull, fnNull, fnNull, fnNull, fnNull, fnNull}
    };

    for(uint8_t i = 0; i < smbusBufferLength; ++i)
    {
        smbus_oldBuffer[i] = smbusBuffer[i];    
    }
    smbus_oldBufferLength = smbusBufferLength;
    for(uint8_t i = 0; i < numBytes; ++i)
    {
        smbusBuffer[i] = inBytes[i];    
    }
    smbusBufferLength = numBytes;
    // skip commands without data and assume they are for read_i2c_block_data

    if( !(smbusBufferLength > 1) ) 
    {
        smbusBuffer[0] = 0xFF; // error code for small size.
        return; // not valid, do nothing just echo an error code.
    }

    // mask the group bits (6 and 7) so they are alone then roll those bits to the left so they can be used as an index.
    uint8_t group;
    group = (smbusBuffer[0] & 0xc0) >> 6;
    if(group >= GROUP) 
    {
        smbusBuffer[0] = 0xFE; // error code for bad group.
        return; //this can not happen... but
    }

    // mask the command bits (0..5) so they can be used as an index.
    uint8_t command;
    command = smbusBuffer[0] & 0x3F;
    if(command >= MGR_CMDS) 
    {
        smbusBuffer[0] = 0xFD; // error code for bad command.
        return; // not valid, do nothing but echo error code.
    }

    // Call the i2c command function and return
    (* pf[group][command])(smbusBuffer);
    return;	
}

// called when SMBus slave has been requested to send data
void transmit1_event(void) 
{
    // For SMBus echo the old data from the previous I2C receive event
    twi1_transmit(smbus_oldBuffer, smbus_oldBufferLength);
}