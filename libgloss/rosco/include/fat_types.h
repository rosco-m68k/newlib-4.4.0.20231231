/*
 * 
 * NOT PUBLIC API!
*/
#ifndef  __ROSCOM68K_LIBC_FAT_TYPES_H__
#define  __ROSCOM68K_LIBC_FAT_TYPES_H__

// Detect 64-bit compilation on GCC
#if defined(__GNUC__) && defined(__SIZEOF_LONG__)
    #if __SIZEOF_LONG__ == 8
        #define FATFS_DEF_UINT32_AS_INT
    #endif
#endif

//-------------------------------------------------------------
// System specific types
//-------------------------------------------------------------
#ifndef FATFS_NO_DEF_TYPES
    typedef unsigned char uint8;
    typedef unsigned short uint16;

    // If compiling on a 64-bit machine, use int as 32-bits
    #ifdef FATFS_DEF_UINT32_AS_INT
        typedef unsigned int uint32;
    // Else for 32-bit machines & embedded systems, use long...
    #else
        typedef unsigned long uint32;
    #endif
#endif

#ifndef NULL
    #define NULL 0
#endif

//-------------------------------------------------------------
// Endian Macros
//-------------------------------------------------------------
// FAT is little endian so big endian systems need to swap words

// Little Endian - No swap required
#if FATFS_IS_LITTLE_ENDIAN == 1

    #define FAT_HTONS(n) (n)
    #define FAT_HTONL(n) (n)

// Big Endian - Swap required
#else

    #define FAT_HTONS(n) ((((uint16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))
    #define FAT_HTONL(n) (((((uint32)(n) & 0xFF)) << 24) | \
                    ((((uint32)(n) & 0xFF00)) << 8) | \
                    ((((uint32)(n) & 0xFF0000)) >> 8) | \
                    ((((uint32)(n) & 0xFF000000)) >> 24))

#endif

//-------------------------------------------------------------
// Structure Packing Compile Options
//-------------------------------------------------------------
#ifdef __GNUC__
    #define STRUCT_PACK
    #define STRUCT_PACK_BEGIN
    #define STRUCT_PACK_END
    #define STRUCT_PACKED           __attribute__ ((packed))
    // ROSCO_M68K - Xark
    // Bug workaround for packed vs alignment gcc-7.5 m68k issue. It tries to
    // "optimize" two byte writes into one mis-aligned word store causing
    // exception.  It may be related to this, but worked around by declaring
    // struct pointer "volatile" which prevents the optimization that is
    // breaking:
    // https://stackoverflow.com/questions/8568432/is-gccs-attribute-packed-pragma-pack-unsafe
    #ifdef __m68k__
        #define STRUCT_PACKED_VOLATILE  volatile
    #else
        #define STRUCT_PACKED_VOLATILE
    #endif
#else
    // Other compilers may require other methods of packing structures
    #define STRUCT_PACK
    #define STRUCT_PACK_BEGIN
    #define STRUCT_PACK_END
    #define STRUCT_PACKED
    #define STRUCT_PACKED_VOLATILE
#endif

#endif
