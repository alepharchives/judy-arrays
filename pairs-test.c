/*
 *  pairs-test.c
 *  judy-arrays
 *
 *  Created by Jan on 18.12.10.
 *  Copyright 2010 geheimwerk.de. All rights reserved.
 *
 */

#include <stdio.h>
#include "judy-utilities.c"

int main(int argc, char **argv) {
	uchar buff[1024];
	uchar key[BOTTOM_UP_SIZE+1] = {0};
	FILE *in, *out;
	
	judyvalue index;					// array index
	judyvalue value;					// array element value
	judyslot *cell;						// pointer to array element value
	
	void *judy;							// pointer to Judy array
	
	if( argc > 1 )
		in = fopen(argv[1], "r");
	else
		in = stdin;
	
	if( argc > 2 )
		out = fopen(argv[2], "w");
	else
		out = stdout;
	
	if( !in )
		fprintf(stderr, "unable to open input file\n");
	
	if( !out )
		fprintf(stderr, "unable to open output file\n");
	
	
#if 0
	judyvalue test = 0;
	
	do {
		index = test;
		judyvalue_native_to_bottom_up(index, key);
		index = judyvalue_bottom_up_to_native(key);
		if (index != test) {
			printf("Encoding error: %"PRIjudyvalue "\n", test);
		}
		test++;
	} while (test != 0);
#endif
	
	judy = judy_open(BOTTOM_UP_MAX_JUDY_STACK_LEVELS);
	
	while( fgets((char *)buff, sizeof(buff), in) ) {
		if (sscanf((char *)buff, "%"PRIjudyvalue " %"PRIjudyvalue, &index, &value)) {
			judyvalue_native_to_bottom_up(index, key);
#define ENABLE_READ_LOGGING	0
#if ENABLE_READ_LOGGING
			for (int i = 0; i < BOTTOM_UP_SIZE; i++) {
				printf("%02x", key[i]);
			}
			printf(" ");
			printf("%"PRIjudyvalue " %"PRIjudyvalue "\n", index, value);
#endif
			cell = judy_cell(judy, key, BOTTOM_UP_SIZE);
			if (value) {
				*cell = value;                 // store new value
			} else {
				*cell = -1;
			}
		}
	}
	
	// Next, visit all the stored indexes in sorted order, first ascending,
	// then descending, and delete each index during the descending pass.
	
	index = 0;
	cell = judy_strt(judy, NULL, 0);
	while (cell != NULL)
	{
		judy_key(judy, key, sizeof(key));
		index = judyvalue_bottom_up_to_native(key);
		
		value = *cell;
		if (value == -1) value = 0;
		printf("%"PRIjudyvalue " %"PRIjudyvalue "\n", index, value);

		cell = judy_nxt(judy);
#define SYMMETRY_TEST	0
#if SYMMETRY_TEST
		cell = judy_prv(judy); // This will work if judy_prv() and judy_nxt() are symmetric. 
		cell = judy_nxt(judy);
#endif
	}

	printf("\n");

	cell = judy_end(judy);
	while (cell != NULL)
	{
		judy_key(judy, key, sizeof(key));
		index = judyvalue_bottom_up_to_native(key);
		
		value = *cell;
		if (value == -1) value = 0;
		printf("%"PRIjudyvalue " %"PRIjudyvalue "\n", index, value);
		
		cell = judy_del(judy);
		//cell = judy_prv(judy);
	}

	return 0;
}