#ifndef SMBUS_cmds_H
#define SMBUS_cmds_H

#define SMBUS_BUFFER_LENGTH 32

extern uint8_t smbusBuffer[SMBUS_BUFFER_LENGTH];
extern uint8_t smbusBufferLength;
extern uint8_t smbus_oldBuffer[SMBUS_BUFFER_LENGTH]; //transmit oldBuffer
extern uint8_t smbus_oldBufferLength;
extern uint8_t transmit_data_ready;

extern void receive1_event(uint8_t*, int);
extern void transmit1_event(void);

#endif // SMBUS_cmds_H 
