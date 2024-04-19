#pragma once

#include "system_vars.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

extern int decode_and_execute(uint32_t ins);
extern int address_to_immediate(uint16_t imm, AddressingMode mode);