/*
state for RPUBUS manager
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
#include "rpubus_manager_state.h"

unsigned long blink_started_at;
unsigned long lockout_started_at;
unsigned long uart_started_at;
unsigned long bootloader_started_at;
unsigned long shutdown_started_at;

uint8_t bootloader_started;
uint8_t host_active;
uint8_t localhost_active;
uint8_t bootloader_address; 
uint8_t lockout_active;
uint8_t uart_has_TTL;
uint8_t host_is_foreign;
uint8_t local_mcu_is_rpu_aware;
uint8_t rpu_address;
uint8_t write_rpu_address_to_eeprom;
uint8_t shutdown_detected;
uint8_t shutdown_started;

volatile uint8_t status_byt;
volatile uint8_t uart_output;
