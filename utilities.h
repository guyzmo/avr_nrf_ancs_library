/** (c)2013, Bernard Pratz, bernard at pratz dot net
 *  under the WTFPL License
 */
#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <Arduino.h>

void free_ram (void);
void serial_print_char(char c);

#ifdef DEBUG1
#define debug_println(...) Serial.println(__VA_ARGS__)
#define debug_print(...) Serial.print(__VA_ARGS__)
#else
#define debug_println(...)
#define debug_print(...)
#endif

#ifdef DEBUG2
#define debug2_println(...) Serial.println(__VA_ARGS__)
#define debug2_print(...) Serial.print(__VA_ARGS__)
#else
#define debug2_println(...)
#define debug2_print(...)
#endif

#ifdef DEBUG3
#define debug3_println(...) Serial.println(__VA_ARGS__)
#define debug3_print(...) Serial.print(__VA_ARGS__)
#else
#define debug3_println(...)
#define debug3_print(...)
#endif

static uint8_t base=0;
inline Print &operator <<(Print &obj, unsigned long arg)
{  
    switch (base) {
        case HEX: obj.print("0x"); break;
        case BIN: obj.print("0b"); break;
    }
    obj.print(arg, (int)base);
    base = 0;
    return obj; 
}
template<class T>
inline Print &operator <<(Print &obj, T arg)
{  
    obj.print(arg); 
    return obj; 
}
inline unsigned long hex(unsigned long arg)
{  
  base = HEX;
  return arg; 
}
inline unsigned long bin(unsigned long arg)
{  
  base = BIN;
  return arg; 
}

#define endl F("\n")
#define cout Serial

#endif
