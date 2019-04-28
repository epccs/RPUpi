#ifndef SMBUS_cmds_H
#define SMBUS_cmds_H

// SMBus Commands
// Host Commands on I2C1 does 0, 2, 3, 6, 7
#define SMBUS_COMMAND_TO_READ_RPU_ADDRESS 0
#define SMBUS_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR 2
#define SMBUS_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR 3
#define SMBUS_COMMAND_TO_READ_STATUS 6
#define SMBUS_COMMAND_TO_SET_STATUS 7

extern uint8_t i2c1Buffer[TWI1_BUFFER_LENGTH];
extern uint8_t i2c1BufferLength;
extern uint8_t i2c1_oldBuffer[TWI1_BUFFER_LENGTH]; //i2c1_old is for SMBus
extern uint8_t i2c1_oldBufferLength;

extern void receive1_event(uint8_t*, int);
extern void transmit1_event(void);

#endif // SMBUS_cmds_H 
