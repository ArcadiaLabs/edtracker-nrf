#include <stdint.h>
#include <compiler_mcs51.h>

#include "rf_protocol.h"

__code const uint8_t HeadAddr[] =	{0x63, 0x4C, 0x70, 0x01, 0x10};
__code const uint8_t DongleAddr[] =	{0x36, 0xC4, 0x31, 0x4B, 0x34};
