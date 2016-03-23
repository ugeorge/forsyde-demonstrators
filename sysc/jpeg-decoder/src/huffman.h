#ifndef _JPEG_HUFFMAN_H_INCLUDED
#define _JPEG_HUFFMAN_H_INCLUDED

#include "stdio-wrapper.h"

#define HUFF_ID(hclass, id)       (2 * (hclass) + (id))

#define DC_CLASS        0
#define AC_CLASS        1

#define HUFF_EOB		0x00
#define HUFF_ZRL		0xF0

/*
 * initialize huffman state
 */
void init_huffman(void);

/*
 * restart VLD stream on next byte
 */
void clear_bits(void);

/*
 * Loading of Huffman table, with leaves drop ability
 */
int load_huff_tables(SYSACE_FILE * fi);

/*
 * extract a single symbol from file
 * using specified huffman table ...
 */
unsigned char get_symbol(SYSACE_FILE * fi, int select);

/*
 * read one bit from file
 */
unsigned char get_one_bit(SYSACE_FILE * fi);

/*
 * read 'number' (1 <= n <= 32) of bits from file
 */
unsigned long get_bits(SYSACE_FILE * fi, int number);

#endif							/* _JPEG_HUFFMAN_H_INCLUDED */
