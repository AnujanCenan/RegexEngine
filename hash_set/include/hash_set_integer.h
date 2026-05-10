#ifndef HASH_SET_INTEGER_H
#define HASH_SET_INTEGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HASH_SET_MIN_SIZE 8
#define MAX_FULLNESS_CAP 0.75
#define TOMBSTONE_NOT_FOUND -1

typedef struct
{
    int key;
    int hash;
    bool tombstone;
} HT_Item;


typedef struct 
{
    HT_Item** ht;
    int num_elements;       // number of active + number of tombstones
    int num_active;
    int total_size;

} Hash_Set;

typedef struct
{
    int* iterable;
    int size;

} HS_Iterable;



Hash_Set* hash_set_init();
void hash_set_add(Hash_Set** hs, int key);
bool hash_set_exists(Hash_Set* hs, int key);
void hash_set_delete(Hash_Set* hs, int key);
void hash_set_free(Hash_Set* hs);
bool hash_sets_equal(Hash_Set* hs1, Hash_Set* hs2);
void hash_set_print(Hash_Set* hs);

HS_Iterable* hash_set_get_iterable(Hash_Set* hs);
void print_iterable(HS_Iterable* iter);
void hash_set_iterable_free(HS_Iterable* iter);

#endif