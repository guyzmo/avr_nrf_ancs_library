/*
helper functions to work with bytearrays

usage:

    pack(unsigned char *buf, const char *fmt, ...);

        similar to sprintf():
        put all arguments in "..." into the buffer, according to the format
        string "fmt", in a way that they can be easily retrieved with unpack()


    unpack(const unsigned char *buf, const char *fmt, ...);

        similar to sscanf():
        retrieves data from the buffer according to "fmt". arguments in "..."
        must be pointers to appropriate types.


    format specifiers:
        b   = int8_t    B   = uint8_t
        h   = int16_t   H   = uint16_t
        i   = int32_t   I   = uint32_t
        l   = int64_t   L   = uint64_t
        _   = skip one byte (mainly useful for unpacking)

    example:
        uint8_t a;
        uint16_t b = 1337;
        int32_t c = -12345678;

        pack(buf, "BBHi", 255, 0, b, c);
        unpack(buf, "B_Hi", &a, &b, &c);

        assert(a == 255);
        assert(b == 1337);
        assert(c == -12345678);

source: http://sprunge.us/LEDM
license: WTFPL
by rob`` on freenode, ##c

 */
#ifndef _PACK_H_
#define _PACK_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef NO_PACK
static size_t pack(unsigned char *buf, const char *fmt, ...);
#endif
#ifndef NO_UNPACK
static size_t unpack(const unsigned char *buf, const char *fmt, ...);
#endif

#ifndef NO_PACK
static void buffer_put(unsigned char *buffer, uint64_t value, size_t bytes);
#endif
#ifndef NO_UNPACK
static uintmax_t buffer_get(const unsigned char *buffer, size_t bytes);
#endif

#define PACK_UNPACK(c, name, CASE)                              \
static size_t name(c unsigned char *buf, const char *fmt, ...)  \
{                                                               \
    va_list ap;                                                 \
    size_t sz, total = 0;                                       \
                                                                \
    va_start(ap, fmt);                                          \
                                                                \
    for (; *fmt; fmt++)                                         \
    {                                                           \
        switch (*fmt)                                           \
        {                                                       \
            CASE('b', int, int8_t);                             \
            CASE('B', int, uint8_t);                            \
                                                                \
            CASE('h', int, int16_t);                            \
            CASE('H', int, uint16_t);                           \
                                                                \
            CASE('i', int32_t, int32_t);                        \
            CASE('I', uint32_t, uint32_t);                      \
                                                                \
            CASE('l', int64_t, int64_t);                        \
            CASE('L', uint64_t, uint64_t);                      \
                                                                \
            case '_':                                           \
                sz = 1;                                         \
                break;                                          \
                                                                \
            default:                                            \
                va_end(ap);                                     \
                return total;                                   \
        }                                                       \
                                                                \
        buf += sz;                                              \
        total += sz;                                            \
    }                                                           \
                                                                \
    va_end(ap);                                                 \
                                                                \
    return total;                                               \
}

#define CASE_PACK(chr, ap_type, type)                           \
    case chr:                                                   \
        {                                                       \
            type val = va_arg(ap, ap_type);                     \
            sz = sizeof (type);                                 \
            buffer_put(buf, val, sz);                           \
        }                                                       \
        break

#define CASE_UNPACK(chr, ap_type, type)                         \
    case chr:                                                   \
        {                                                       \
            type *p = va_arg(ap, type*);                        \
            sz = sizeof (type);                                 \
            *p = buffer_get(buf, sz);                           \
        }                                                       \
        break

#ifndef NO_PACK
/* define pack() */
PACK_UNPACK(, pack, CASE_PACK)
#endif

#ifndef NO_UNPACK
/* define unpack() */
PACK_UNPACK(const, unpack, CASE_UNPACK)
#endif

#ifdef PACK_LITTLE_ENDIAN
// LITTLE ENDIAN VERSION
#ifndef NO_PACK
static void buffer_put(unsigned char *buffer, uint64_t value, size_t bytes)
{
    size_t i = 0;
    while (bytes-(i++)) {
        buffer[i-1] = value & 0xFF;
        value >>= 8;
    }
}
#endif

#ifndef NO_UNPACK
static uint64_t buffer_get(const unsigned char *buffer, size_t bytes)
{
    uint64_t value = 0;
    const unsigned char *end = buffer + bytes;

    while (buffer < end) {
        value <<= 8;
        value += *(--end);
    }

    return value;
}
#endif
#else
// BIG ENDIAN version
#ifndef NO_PACK
static void buffer_put(unsigned char *buffer, uint64_t value, size_t bytes)
{
    while (bytes--) {
        buffer[bytes] = value & 0xFF;
        value >>= 8;
    }
}
#endif

#ifndef NO_UNPACK
static uint64_t buffer_get(const unsigned char *buffer, size_t bytes)
{
    uint64_t value = 0;
    const unsigned char *end = buffer + bytes;

    while (buffer < end) {
        value <<= 8;
        value += *buffer++;
    }

    return value;
}
#endif // NO_UNPACK

#endif // ENDIAN

#endif // PACK_H

