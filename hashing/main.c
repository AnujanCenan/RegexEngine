#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "../dynamic_array/dynamic_array.h"
// Goals:

// struct for the hash table
// Fields:
//      - the 64-bit key table thing
//      - the actual hash table
// Given an upper bound, generate sufficient 64-bit keys and store it in a key
// Then for a particular <set> of integers, do the xor-ing thing
// Get the XOR-result and mod it to get the valid index
    // While the 'spot' is occupied, move to the next (linear probe)
    // Add to the next empty spot



// For now assume resizing will not be necessary

// SETS need to be sorted for this to work (comparison is done linearly)
DECLARE_DYNAMIC_ARRAY(int, Set);

#define HT_SIZE 1024

typedef struct
{
    Set* set;
    bool tombsone;
} Zobrist_Element;


typedef struct
{
    uint64_t* random_key_table;
    Zobrist_Element** ht;
    int upper_bound;
    int num_elements;
    int capacity;
} Zobrist;


// private
uint64_t generate_random_key()
{
    return (uint64_t) rand() << 32 | (uint64_t) rand();
}

// private
uint64_t* generate_random_key_table(int upper_bound)
{
    uint64_t* table = malloc((upper_bound + 1) * sizeof(uint64_t));
    for (int i = 0; i <= upper_bound; ++i)
    {
        table[i] = generate_random_key();           // Assuming no collisions occur
    }

    return table;
}


Zobrist* zobrist_init(int upper_bound)
{
    Zobrist* zobrist = malloc(sizeof(Zobrist));
    zobrist->upper_bound = upper_bound;
    zobrist->random_key_table = generate_random_key_table(upper_bound);
    zobrist->ht = calloc(HT_SIZE, sizeof(Zobrist_Element**));
    zobrist->capacity = HT_SIZE;
    zobrist->num_elements = 0;

    return zobrist;
}


// private
int find_next_available(int start, Zobrist* zobrist)
{
    int num_collisions = 0;

    int i = start;
    for (int count = 0; count < zobrist->capacity; ++count)
    {
        if (zobrist->ht[i] == NULL || zobrist->ht[i]->tombsone)
        {
            printf("\tNumber of collisions: %d\n", num_collisions);
            return i;
        }

        ++num_collisions;   
        ++i;
    }

    fprintf(stderr, "Hashing::find_next_available: No way, ran out of space :(\n");
    exit(1);
}

// private
uint64_t zobrist_hash(Zobrist* zobrist, Set* s)
{
    uint64_t h = 0;
    for (int i = 0; i < s->size; ++i)
    {
        int curr_element = s->data[i];
        if (curr_element > zobrist->upper_bound)
        {
            fprintf(stderr, "Hashing::zobrist_add: Element %d in set is larger than the Zobrist upper bound\n", curr_element);
            exit(1);
        }

        h ^= zobrist->random_key_table[curr_element];
    }

    return h;
}

void zobrist_add(Zobrist* zobrist, Set* s)
{
    uint64_t h = zobrist_hash(zobrist, s);

    int index = (int) (h & (zobrist->capacity - 1));
    printf("Pushing next set:\n");
    index = find_next_available(index, zobrist);
    
    Zobrist_Element* elmt = malloc(sizeof(Zobrist_Element));
    elmt->set = s;
    elmt->tombsone = false;

    zobrist->ht[index] = elmt;
    zobrist->num_elements++;
}

// private
bool sets_match(Set* s1, Set* s2)
{
    if (s1 == NULL || s2 == NULL)
    {
        printf("Hashing::sets_match: (WARNING) - either s1 or s2 is NULL; unadvisable to use the NULL set for this\n");
        return false;
    }

    if (s1->size != s2->size) return false;


    for (int i = 0; i < s1->size; ++i)
    {
        if (s1->data[i] != s2->data[i]) return false;
    }

    return true;
}

// private
bool find_set(Zobrist* zobrist, Set* s, int start)
{
    int num_misses = 0;

    int i = start;
    for (int count = 0; count < zobrist->capacity; ++count)
    {

        if (i == zobrist->capacity) i = 0;

        if (zobrist->ht[i] == NULL) return false;


        if (!zobrist->ht[i]->tombsone && sets_match(s, zobrist->ht[i]->set))
        {
            printf("\tNumber of misses = %d\n", num_misses);
            return true;
        }

        ++num_misses;
        ++i;
    }

    return false;
}

bool zobrist_exists(Zobrist* zobrist, Set* s)
{
    uint64_t h = zobrist_hash(zobrist, s);
    int index = (int) (h & (zobrist->capacity - 1));

    bool result = find_set(zobrist, s, index);

    if (result) printf("Found!\n");
    else printf("Not Found\n");

    return result;

}

int main()
{

    // Ensure Set is always sorted (in order for comparison to be more efficient)


    Set* s1 = Set_init(8);
    Set_append(s1, 1);
    Set_append(s1, 3);
    Set_append(s1, 5);
    Set_append(s1, 6);
    Set_append(s1, 8);
    Set_append(s1, 11);

    Set* s2 = Set_init(8);
    Set_append(s2, 5);
    Set_append(s2, 12);
    Set_append(s2, 13);
    Set_append(s2, 14);
    Set_append(s2, 22);
    Set_append(s2, 24);
    Set_append(s2, 26);

    Set* s3 = Set_init(8);
    Set_append(s3, 211);
    Set_append(s3, 312);
    Set_append(s3, 332);
    Set_append(s3, 340);
    Set_append(s3, 341);
    Set_append(s3, 342);
    Set_append(s3, 350);

    Zobrist* z = zobrist_init(350);
    zobrist_add(z, s1);
    zobrist_add(z, s2);
    zobrist_add(z, s3);

    zobrist_exists(z, s1);      // Found!
    zobrist_exists(z, s2);      // Found!
    zobrist_exists(z, s3);      // Found!

    Set* s4 = Set_init(6);
    Set_append(s4, 8);
    Set_append(s4, 12);
    Set_append(s4, 15);
    Set_append(s4, 33);
    Set_append(s4, 34);
    Set_append(s4, 35);
    Set_append(s4, 37);

    zobrist_exists(z, s4);      // Not found

    Set* s5 = Set_init(8);
    Set_append(s5, 211);
    Set_append(s5, 312);
    Set_append(s5, 332);
    Set_append(s5, 340);
    Set_append(s5, 341);
    Set_append(s5, 342);
    Set_append(s5, 350);

    zobrist_exists(z, s5);      // Found!


    printf("Num elmts in zobrist = %d\n", z->num_elements);

    return 0;
}