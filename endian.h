
/* endian.h */

#ifndef _ENDIAN_H
#define _ENDIAN_H

#if defined _WIN32 || defined __CYGWIN__
    // https://docs.microsoft.com/en-us/windows/win32/api/winsock2/
    #include <winsock2.h>
#else
    // https://pubs.opengroup.org/onlinepubs/7908799/xns/arpainet.h.html
    #include <arpa/inet.h>
#endif

// host to big endian
#define __htobe32(...) htonll(__VA_ARGS__)
#define __htobe16(...) htons(__VA_ARGS__)

// host to little endian
#define __htole32(...) __builtin_bswap32(htonl(__VA_ARGS__))
#define __htole16(...) __builtin_bswap16(htons(__VA_ARGS__))

// big endian to host
#define __betoh32(...) ntohll(__VA_ARGS__)
#define __betoh16(...) ntohs(__VA_ARGS__)

// little endian to host
#define __letoh32(...) __builtin_bswap32(ntohl(__VA_ARGS__))
#define __letoh16(...) __builtin_bswap16(ntohs(__VA_ARGS__))

#endif
