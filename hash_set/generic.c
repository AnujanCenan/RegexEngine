/**
 * Generic means: key needs to be a void*
 * User needs to give a hash function pointer (how do I hash this key?)
 * User needs to give a equality function pointer (how do I check if two keys are equal?)
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define HASH_SET_MIN_SIZE 8
#define MAX_FULLNESS_CAP 0.75
#define TOMBSTONE_NOT_FOUND -1

typedef struct
{
    void* key;
    int hash;
    bool tombstone;
} HT_Item;



typedef int (*Hash_Fn) (void* key);
typedef bool (*Equality_Fn) (void* key1, void* key2);
typedef void (*Free_Key_Fn) (void* key1);


typedef void (*PrintItem_Fn) (void* key1);
typedef struct 
{
    HT_Item** ht;
    Hash_Fn get_hash;
    Equality_Fn is_equal;
    Free_Key_Fn free_key;

    int num_elements;       // number of active + number of tombstones
    int num_active;
    int total_size;
} Hash_Set;


typedef struct
{
    void** iterable;        // an array of void* where each void* refers to a key
    int size;

    PrintItem_Fn print_item;

} HS_Iterable;


Hash_Set* hash_set_init(Hash_Fn hash_fn, Equality_Fn equality_fn, Free_Key_Fn free_key)
{
    Hash_Set* hs = malloc(sizeof(Hash_Set));
    hs->ht = calloc(HASH_SET_MIN_SIZE,  sizeof(HT_Item*));
    hs->total_size = HASH_SET_MIN_SIZE;
    hs->num_active = 0;
    hs->num_elements = 0;
    hs->get_hash = hash_fn;
    hs->is_equal = equality_fn;
    hs->free_key = free_key;

    return hs;
}

Hash_Set* hash_set_resize(Hash_Set* hs)
{
    if (1.0 * hs->num_elements / hs->total_size < 0.75)
    {
        return hs;
    }

    Hash_Set* hs_new = malloc(sizeof(Hash_Set));
    hs_new->ht = calloc((hs->total_size << 1), sizeof(HT_Item*));
    hs_new->total_size = hs->total_size << 1;
    hs_new->num_elements = hs->num_active;
    hs_new->num_active = hs->num_active;
    hs_new->get_hash = hs->get_hash;
    hs_new->is_equal = hs->is_equal;


    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i] == NULL) continue;
        else if (hs->ht[i]->tombstone)
        {
            hs->free_key(hs->ht[i]->key);
            free(hs->ht[i]);
        } else
        {
            int index = (hs->ht[i]->hash) & (hs_new->total_size - 1);

            HT_Item* item = hs->ht[i];
            for (int count = 0; count < hs_new->total_size; ++count)
            {
                if (index >= hs_new->total_size) index = 0;
                if (hs_new->ht[index] == NULL)
                {
                    hs_new->ht[index] = item;
                    break;
                }
                ++index;
            }
        }
    }

    free(hs->ht);
    free(hs);

    return hs_new;
}


void hash_set_add(Hash_Set** hs, void* key)
{
    // Check for existence of this key
    if (1.0 * (*hs)->num_elements / (*hs)->total_size >= MAX_FULLNESS_CAP)
    {
        (*hs) = hash_set_resize((*hs));
    }

    int hash = (*hs)->get_hash(key);
    int start = hash & ((*hs)->total_size - 1);

    int index = start;
    int first_tombstone = TOMBSTONE_NOT_FOUND;       // -1 means not found here
    bool already_exists = false;
    for (int count = 0; count < (*hs)->total_size; ++count)
    {
        if (index == (*hs)->total_size) index = 0;
        if ((*hs)->ht[index] == NULL) break;

        void* curr_key = (*hs)->ht[index];
        if (first_tombstone == TOMBSTONE_NOT_FOUND && (*hs)->ht[index]->tombstone)
        {
            first_tombstone = index;
        } else if ((*hs)->is_equal(curr_key, key)) 
        {
            already_exists = true;
            break;
        }
        ++index;
    }

    if (already_exists) return;

    HT_Item* item = malloc(sizeof(HT_Item));
    item->tombstone = false;
    item->hash = hash;
    item->key = key;

    if (first_tombstone == TOMBSTONE_NOT_FOUND)
    {
        (*hs)->ht[index] = item;
    } else
    {
        HT_Item* tombstone_item = (*hs)->ht[first_tombstone];
        (*hs)->free_key(tombstone_item->key);
        free(tombstone_item);
        (*hs)->ht[first_tombstone] = item;
    }

    (*hs)->ht[first_tombstone == TOMBSTONE_NOT_FOUND ? index : first_tombstone] = item;

    (*hs)->num_active++;
    (*hs)->num_elements++;

    return;
}

bool hash_set_exists(Hash_Set* hs, void* key)
{
    int hash = hs->get_hash(key);
    int start = hash & (hs->total_size - 1);

    int index = start;
    for (int count = 0; count < hs->total_size; ++count)
    {
        if (index >= hs->total_size) index = 0;

        if (hs->ht[index] == NULL) return false;

        void* curr_key = hs->ht[index]->key;
        
        if (!hs->ht[index]->tombstone && hs->is_equal(curr_key, key)) return true;

        ++index;
    }

    return false;
}

void hash_set_delete(Hash_Set* hs, void* key)
{
    int hash = hs->get_hash(key);
    int start = hash & (hs->total_size - 1);

    int index = start;
    for (int count = 0; count < hs->total_size; ++count)
    {
        if (index >= hs->total_size) index = 0;

        if (hs->ht[index] == NULL) return;
        
        void* curr_key = hs->ht[index]->key;
        if (!hs->ht[index]->tombstone && hs->is_equal(curr_key, key))
        {
            hs->ht[index]->tombstone = true;
            return;
        }

        ++index;
    }

    return;
}

HS_Iterable* hash_set_get_iterable(Hash_Set* hs, PrintItem_Fn print_item)
{
    HS_Iterable* iter = malloc(sizeof(HS_Iterable));
    iter->iterable = malloc(hs->total_size * sizeof(void*));
    iter->size = 0;
    iter->print_item = print_item;

    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i] == NULL || hs->ht[i]->tombstone) continue;

        iter->iterable[iter->size++] = hs->ht[i]->key;
    }

    return iter;
}

void print_iterable(HS_Iterable* iter)
{
    printf("Printing iterable:");
    if (iter->size == 0)
    {
        printf("Empty\n");
        return;
    }

    printf("\n");
    for (int i = 0; i < iter->size; ++i)
    {
        iter->print_item(iter->iterable[i]);
        printf("\n");
    }
}

void hash_set_free(Hash_Set* hs)
{
    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i]) {
            HT_Item* item = hs->ht[i];
            hs->free_key(item->key);
            free(item);
        }
    }
    free(hs->ht);
    free(hs);
}

void iterable_free(HS_Iterable* iter)
{
    free(iter->iterable);
    free(iter);
}

///////// User Perspective //////////

int hash(void* key)
{
    int casted_key = *((int *) key);
    return casted_key;
}

bool equality(void* key1, void* key2)
{
    int casted_key1 =  *((int *) key1);
    int casted_key2 =  *((int *) key2);

    return casted_key1 == casted_key2;
}

void print(void* key)
{
    int casted_key = *((int *) key);
    printf("%d", casted_key);
}

void free_key(void* key)
{
    return;
}


// To avoid some pain
void hs_add_int(Hash_Set** set, int key)
{
    hash_set_add(set, &key);
}

bool hs_exists_int(Hash_Set* set, int key)
{
    return hash_set_exists(set, &key);
}

void hs_delete_int(Hash_Set* set, int key)
{
    hash_set_delete(set, &key);
}


int main()
{
    Hash_Set* set = hash_set_init(&hash, &equality, &free_key);
    int a = 23;
    int b = 15;
    int c = 36;
    int d = 75;
    int e = 43;
    int f = 12;
    int g = 94;

    hash_set_add(&set, &a);
    hash_set_add(&set, &b);
    hash_set_add(&set, &c);
    hash_set_add(&set, &d);
    hash_set_add(&set, &e);
    hash_set_add(&set, &f);
    hash_set_add(&set, &g);


    // hs_add_int(&set, 23);       // % == 7
    // hs_add_int(&set, 15);       // 7
    // hs_add_int(&set, 36);       // 4
    // hs_add_int(&set, 75);       // 3
    // hs_add_int(&set, 43);       // 3
    // hs_add_int(&set, 12);       // 4
    // hs_add_int(&set, 94);

    // printf("Exists(36) = %d\n", hs_exists_int(set, 36));
    // printf("Exists(75) = %d\n", hs_exists_int(set, 75));
    // printf("Exists(94) = %d\n", hs_exists_int(set, 94));
    // printf("Exists(43) = %d\n", hs_exists_int(set, 43));

    printf("Exists(36) = %d\n", hash_set_exists(set, &c));
    printf("Exists(75) = %d\n", hash_set_exists(set, &d));
    printf("Exists(94) = %d\n", hash_set_exists(set, &g));
    printf("Exists(43) = %d\n", hash_set_exists(set, &e));


    int v = 1;
    int w = 2;
    int x = 3; 
    int y = 4;
    int z = 5;
    
    printf("Exists(1) = %d\n", hash_set_exists(set, &v));
    printf("Exists(2) = %d\n", hash_set_exists(set, &w));
    printf("Exists(3) = %d\n", hash_set_exists(set, &x));
    printf("Exists(4) = %d\n", hash_set_exists(set, &y));
    printf("Exists(5) = %d\n", hash_set_exists(set, &z));


    printf("Before delete: Exists(23) = %d\n", hash_set_exists(set, &a));
    hash_set_delete(set, &a);
    printf("After delete: Exists(23) = %d\n", hash_set_exists(set, &a));


    HS_Iterable* iter = hash_set_get_iterable(set, &print);
    print_iterable(iter);

    
    return 0;
}
