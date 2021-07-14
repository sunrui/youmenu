/* adler32.h -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */
#ifndef ADLER32_H
#define ADLER32_H

/*
     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
   return the updated checksum. If buf is NULL, this function returns
   the required initial value for the checksum.
   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
   much faster. Usage example:

     unsigned long adler = adler32(0L, 0, 0);

     while (read_buffer(buffer, length) != EOF) {
       adler = adler32(adler, buffer, length);
     }
     if (adler != original_adler) error();
*/

extern unsigned long adler32(unsigned long crc, unsigned char *buf, unsigned int len);

#endif