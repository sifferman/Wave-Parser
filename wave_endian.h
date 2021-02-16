
/* wave_endian.h */

#ifndef _WAVE_ENDIAN_H
#define _WAVE_ENDIAN_H

#if defined _WIN32 || defined __CYGWIN__
    // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/
    #include <winsock2.h>
#else
    // https://pubs.opengroup.org/onlinepubs/7908799/xns/arpainet.h.html
    #include <arpa/inet.h>
#endif

// host to big endian
#define __htobe32(...) htonl(__VA_ARGS__)
#define __htobe16(...) htons(__VA_ARGS__)

// host to little endian
#define __htole32(...) __builtin_bswap32(htonl(__VA_ARGS__))
#define __htole16(...) __builtin_bswap16(htons(__VA_ARGS__))

// big endian to host
#define __betoh32(...) ntohl(__VA_ARGS__)
#define __betoh16(...) ntohs(__VA_ARGS__)

// little endian to host
#define __letoh32(...) __builtin_bswap32(ntohl(__VA_ARGS__))
#define __letoh16(...) __builtin_bswap16(ntohs(__VA_ARGS__))




/*=================== Helpers =====================*/

/*
 * Exception for if load_le() is attempted on a data type with
 * incompatible size.
 */
class BadSize { };

/*
 * Loads little endian data from buffer + offset into the destination with
 * the correct endianness.
 */
template <class T>
void load_le( T & destination, const uint8_t * const buffer, uint32_t & offset ) {
    uint8_t length = sizeof(destination);
    memcpy( &destination, buffer + offset, length );
    switch ( length ) {
    case 1:
        break;
    case 2:
        destination = __letoh16(destination);
        break;
    case 4:
        destination = __letoh32(destination);
        break;
    default:
        throw BadSize();
        break;
    }
    offset += length;
}

/*
 * Writes little endian data from source into buffer + offset with the
 * correct endianness.
 */
template <class T>
void write_le( const T & source, uint8_t * const buffer, uint32_t & offset ) {
    uint8_t length = sizeof(source);
    memcpy( buffer + offset, &source, length );
    switch ( length ) {
    case 1:
        break;
    case 2:
        *((T*)buffer) = __htole16(*((T*)buffer));
        break;
    case 4:
        *((T*)buffer) = __htole32(*((T*)buffer));
        break;
    default:
        throw BadSize();
        break;
    }
    offset += length;
}




#endif
