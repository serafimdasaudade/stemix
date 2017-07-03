#include <GenericTypeDefs.h>

UINT8 read_eep(UINT badd);
void write_eep(UINT badd, UINT8 bdat);

#define DEFAULT_ADD     0x55
#define EE_DEFAULT_ADD  0x00
#define EE_TIME_SEC    0x01
#define EE_TIME_MINUTE  0x02
#define EE_RPM          0x03
#define EE_TEMP_MSB     0x04
#define EE_TEMP_LSB     0x05