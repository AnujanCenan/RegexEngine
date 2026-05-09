#include "../include/zobrist_hashing.h"

struct Zobrist_Element
{
    Set* set;
    bool tombsone;
} ;


struct Zobrist
{
    uint64_t* random_key_table;
    Zobrist_Element** ht;
    int upper_bound;
    int num_elements;
    int capacity;
} ;


uint64_t generate_random_key();
uint64_t* generate_random_key_table(int upper_bound);
int find_next_available(int start, Zobrist* zobrist);
uint64_t zobrist_hash(Zobrist* zobrist, Set* s);
bool find_set(Zobrist* zobrist, Set* s, int start);


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
    HS_Iterable* iter = hash_set_get_iterable(s);
    for (int i = 0; i < iter->size; ++i)
    {
        int curr_element = iter->iterable[i];
        if (curr_element > zobrist->upper_bound)
        {
            fprintf(stderr, "Hashing::zobrist_add: Element %d in set is larger than the Zobrist upper bound\n", curr_element);
            exit(1);
        }

        h ^= zobrist->random_key_table[curr_element];
    }

    hash_set_iterable_free(iter);

    return h;
}

void zobrist_add(Zobrist* zobrist, Set* s)
{
    uint64_t h = zobrist_hash(zobrist, s);

    int index = (int) (h & (zobrist->capacity - 1));
    index = find_next_available(index, zobrist);
    
    Zobrist_Element* elmt = malloc(sizeof(Zobrist_Element));
    elmt->set = s;
    elmt->tombsone = false;

    zobrist->ht[index] = elmt;
    zobrist->num_elements++;
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

        if (!zobrist->ht[i]->tombsone && hash_sets_equal(s, zobrist->ht[i]->set))
        {
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
    return result;
}


void zobrist_free(Zobrist* z)
{
    free(z->random_key_table);

    for (int i = 0; i < z->capacity; ++i)
    {
        if (z->ht[i] == NULL) continue;
        hash_set_free(z->ht[i]->set);
    }

    free(z->ht);
    free(z);
}


int get_num_elements(Zobrist* z)
{
    return z->num_elements;
}
