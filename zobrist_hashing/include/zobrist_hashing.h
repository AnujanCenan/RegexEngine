#ifndef ZOBRIST_HASHING_H
#define ZOBRIST_HASHING_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../../hash_set/include/primitive.h"

#define HT_SIZE 1024


typedef Hash_Set Set;
typedef struct Zobrist_Element Zobrist_Element;
typedef struct Zobrist Zobrist;


Zobrist* zobrist_init(int upper_bound);
void zobrist_add(Zobrist* zobrist, Set* s, void* value);
bool zobrist_exists(Zobrist* zobrist, Set* s);
void zobrist_free(Zobrist* z);
int get_num_elements(Zobrist* z);

void* zobrist_get(Zobrist* zobrist, Set* s);

#endif