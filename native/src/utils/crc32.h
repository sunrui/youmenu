/* crc32.h -- compute the CRC-32 of a data stream
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */
#ifndef CRC32_H
#define CRC32_H

/*
     Update a running crc with the bytes buf[0..len-1] and return the updated
   crc. If buf is NULL, this function returns the required initial value
   for the crc (0). Pre- and post-conditioning (one's complement) is performed
   within this function so it shouldn't be done by the application.
   Usage example:

     unsigned long crc = crc32(0L, 0, 0);

     while (read_buffer(buffer, length) != EOF) {
       crc = crc32(crc, buffer, length);
     }
     if (crc != original_crc) error();
*/
extern unsigned long crc32(unsigned long crc, unsigned char *buf, unsigned int len);

#endif