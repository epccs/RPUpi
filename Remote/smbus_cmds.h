#ifndef SMBUS_cmds_H
#define SMBUS_cmds_H

#define SMBUS_BUFFER_LENGTH 32

// SMBus Commands
// Host Commands on I2C1 does 0, 2, 3, 6, 7
#define SMBUS_COMMAND_TO_READ_RPU_ADDRESS 0
#define SMBUS_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR 2
#define SMBUS_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR 3
#define SMBUS_COMMAND_TO_READ_STATUS 6
#define SMBUS_COMMAND_TO_SET_STATUS 7

extern uint8_t smbusBuffer[SMBUS_BUFFER_LENGTH];
extern uint8_t smbusBufferLength;
extern uint8_t smbus_oldBuffer[SMBUS_BUFFER_LENGTH]; //transmit oldBuffer
extern uint8_t smbus_oldBufferLength;

extern void receive1_event(uint8_t*, int);
extern void transmit1_event(void);

#endif // SMBUS_cmds_H 
