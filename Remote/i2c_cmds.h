#ifndef I2C_cmds_H
#define I2C_cmds_H

// I2C Commands
#define I2C_COMMAND_TO_READ_RPU_ADDRESS 0
#define I2C_COMMAND_TO_SET_RPU_ADDRESS 1
#define I2C_COMMAND_TO_READ_ADDRESS_SENT_ON_ACTIVE_DTR 2
#define I2C_COMMAND_TO_SET_ADDRESS_SENT_ON_ACTIVE_DTR 3
#define I2C_COMMAND_TO_READ_SW_SHUTDOWN_DETECTED 4
#define I2C_COMMAND_TO_SET_SW_FOR_SHUTDOWN 5
#define I2C_COMMAND_TO_READ_STATUS 6
#define I2C_COMMAND_TO_SET_STATUS 7

extern uint8_t i2c0Buffer[TWI0_BUFFER_LENGTH];
extern uint8_t i2c0BufferLength;

extern void receive_i2c_event(uint8_t*, int);
extern void transmit_i2c_event(void);

#endif // I2C_cmds_H 
