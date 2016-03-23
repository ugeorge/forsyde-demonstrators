#include "stdio-wrapper.h"
//#include <stdlib.h>
#include "huffman.h"
#include "utilities.h"

#define MAX_SIZE(hclass)		((hclass)?162:14)

static unsigned char DC_Table0[MAX_SIZE(DC_CLASS)], DC_Table1[MAX_SIZE(DC_CLASS)];

static unsigned char AC_Table0[MAX_SIZE(AC_CLASS)], AC_Table1[MAX_SIZE(AC_CLASS)];

static unsigned char *HTable[4] = {
	&DC_Table0[0], &DC_Table1[0],
	&AC_Table0[0], &AC_Table1[0]
};

static int MinCode[4][16];
static int MaxCode[4][16];
static int ValPtr[4][16];

static unsigned char bit_count;	/* available bits in the window */
static unsigned int stuffers;
static unsigned char window;

void init_huffman(void)
{
	stuffers = 0;
	bit_count = 0;
}

void clear_bits(void)
{
	bit_count = 0;
}

int load_huff_tables(SYSACE_FILE * fi)
{
	char aux;
	int size, hclass, id, max;
	int LeavesN, LeavesT, i;
	int AuxCode;

	size = get_size(fi);		/* this is the tables' size */
	size -= 2;

	while (size > 0) {
		aux = sysace_fgetc(fi);
		hclass = first_quad(aux);	/* AC or DC */
		id = second_quad(aux);	/* table no */
		if (id > 1) {
			fprintf(stderr, "\tERROR:\tBad HTable identity %d!\n", id);
			return -1;
		}
		id = HUFF_ID(hclass, id);
#ifdef VERBOSE
		fprintf(stderr, "\tINFO:\tLoading Table %d\n", id);
#endif
		size--;

		LeavesT = 0;
		AuxCode = 0;

		for (i = 0; i < 16; i++) {
			LeavesN = sysace_fgetc(fi);
#define DEBUG
#ifdef DEBUG
			// show tables
			fprintf(stderr, "\tCodes of length %d: %d\n", i, LeavesN);
#endif
			ValPtr[id][i] = LeavesT;
			MinCode[id][i] = AuxCode * 2;
			AuxCode = MinCode[id][i] + LeavesN;

			MaxCode[id][i] = (LeavesN) ? (AuxCode - 1) : (-1);
			LeavesT += LeavesN;
		}
		size -= 16;

		if (LeavesT > MAX_SIZE(hclass)) {
			max = MAX_SIZE(hclass);
			fprintf(stderr, "\tWARNING:\tTruncating Table by %d symbols\n", LeavesT - max);
		} else
			max = LeavesT;

		/* load in raw order */
		for (i = 0; i < max; i++)
			HTable[id][i] = sysace_fgetc(fi);

		/* skip if we don't load */
		for (i = max; i < LeavesT; i++)
			sysace_fgetc(fi);
		size -= LeavesT;

#ifdef VERBOSE
		fprintf(stderr, "\tINFO:\tUsing %d words of table memory\n", LeavesT);
#endif
	}							/* loop on tables */
	return 0;
}

unsigned char get_symbol(SYSACE_FILE * fp, int select)
{
	long code = 0;
	int length;
	int index;

	for (length = 0; length < 16; length++) {
		code = (code << 1) | get_one_bit(fp);

		if (code <= MaxCode[select][length])
			break;
	}

	index = ValPtr[select][length] + code - MinCode[select][length];

	if (index < MAX_SIZE(select / 2))
		return HTable[select][index];

	fprintf(stderr, "%ld:\tWARNING:\tOverflowing symbol table !\n", sysace_ftell(fp));
	return 0;
}

unsigned char get_one_bit(SYSACE_FILE * fi)
{
	int newbit;
	unsigned char aux, wwindow;

	if (bit_count == 0) {
		wwindow = sysace_fgetc(fi);

		if (wwindow == 0xFF)
			switch (aux = sysace_fgetc(fi)) {	/* skip stuffer 0 byte */
			case 0xFF:
				fprintf(stderr, "%ld:\tERROR:\tRan out of bit stream\n", sysace_ftell(fi));
				aborted_stream(fi);
				break;

			case 0x00:
				stuffers++;
				break;

			default:
				if (RST_MK(0xFF00 | aux))
					fprintf(stderr, "%ld:\tERROR:\tSpontaneously found restart!\n", sysace_ftell(fi));
				fprintf(stderr, "%ld:\tERROR:\tLost sync in bit stream\n", sysace_ftell(fi));
				aborted_stream(fi);
				break;
			}

		bit_count = 8;
	} else
		wwindow = window;

	newbit = (wwindow >> 7) & 1;
	window = wwindow << 1;
	bit_count--;
	return newbit;
}

unsigned long get_bits(SYSACE_FILE * fi, int number)
{
	int i;
	unsigned long result = 0;

	if (number < 1 || number > 32) {
		fprintf(stderr, "Can't get %d bits at once...\n", number);
		//exit(0);
	}

	for (i = 0; i < number; i++) {
		result = (result << 1) | get_one_bit(fi);
	}
	return result;
}
