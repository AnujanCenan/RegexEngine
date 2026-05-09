#include "../include/primitive.h"

Hash_Set* hash_set_resize(Hash_Set* hs);
int hs_hash(int key);
void print_iterable(HS_Iterable* iter);     // Debugging function


Hash_Set* hash_set_init()
{
    Hash_Set* hs = malloc(sizeof(Hash_Set));
    hs->ht = calloc(HASH_SET_MIN_SIZE,  sizeof(HT_Item*));
    hs->total_size = HASH_SET_MIN_SIZE;
    hs->num_active = 0;
    hs->num_elements = 0;

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

    // hs->total_size <<= 1;

    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i] == NULL) continue;
        else if (hs->ht[i]->tombstone)
        {
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

int hs_hash(int key)
{
    return key;
}

void hash_set_add(Hash_Set** hs, int key)
{
    // Check for existence of this key
    if (1.0 * (*hs)->num_elements / (*hs)->total_size >= MAX_FULLNESS_CAP)
    {
        (*hs) = hash_set_resize((*hs));
    }

    int hash = hs_hash(key);
    int start = hash & ((*hs)->total_size - 1);

    int index = start;
    int first_tombstone = TOMBSTONE_NOT_FOUND;       // -1 means not found here
    bool already_exists = false;
    for (int count = 0; count < (*hs)->total_size; ++count)
    {
        if (index == (*hs)->total_size) index = 0;

        if ((*hs)->ht[index] == NULL)
        {
            break;
        } else if (first_tombstone == TOMBSTONE_NOT_FOUND && (*hs)->ht[index]->tombstone)
        {
            first_tombstone = index;
        } else if ((*hs)->ht[index]->key == key) {
            already_exists = true;
            break;
        }
        ++index;
    }

    if (already_exists)
    {
        printf("Item %d already exists\n", key);
        return;
    } 

    HT_Item* item = malloc(sizeof(HT_Item));
    item->tombstone = false;
    item->hash = hash;
    item->key = key;

    if (first_tombstone == TOMBSTONE_NOT_FOUND)
    {
        (*hs)->ht[index] = item;
    } else
    {
        free((*hs)->ht[first_tombstone]);
        (*hs)->ht[first_tombstone] = item;
    }

    (*hs)->num_active++;
    (*hs)->num_elements++;

    return;
}

bool hash_set_exists(Hash_Set* hs, int key)
{
    int hash = hs_hash(key);
    int start = hash & (hs->total_size - 1);

    int index = start;
    for (int count = 0; count < hs->total_size; ++count)
    {
        if (index >= hs->total_size) index = 0;

        if (hs->ht[index] == NULL) return false;
        
        if (!hs->ht[index]->tombstone && hs->ht[index]->key == key) return true;

        ++index;
    }

    return false;
}

void hash_set_delete(Hash_Set* hs, int key)
{
    int hash = hs_hash(key);
    int start = hash & (hs->total_size - 1);

    int index = start;
    for (int count = 0; count < hs->total_size; ++count)
    {
        if (index >= hs->total_size) index = 0;

        if (hs->ht[index] == NULL) return;
        
        if (!hs->ht[index]->tombstone && hs->ht[index]->key == key)
        {
            hs->ht[index]->tombstone = true;
            return;
        }

        ++index;
    }

    return;
}

bool hash_sets_equal(Hash_Set* hs1, Hash_Set* hs2)
{
    HS_Iterable* iter1 = hash_set_get_iterable(hs1);

    HS_Iterable* iter2 = hash_set_get_iterable(hs2);

    if (iter1->size != iter2->size) return false;

    for (int i = 0; i < iter1->size; ++i)
    {
        if (!hash_set_exists(hs2, iter1->iterable[i])) return false;
        if (!hash_set_exists(hs1, iter2->iterable[i])) return false;
    }

    return true;
}


HS_Iterable* hash_set_get_iterable(Hash_Set* hs)
{
    HS_Iterable* iter = malloc(sizeof(HS_Iterable));
    iter->iterable = malloc(hs->total_size * sizeof(int));
    iter->size = 0;

    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i] == NULL || hs->ht[i]->tombstone) continue;

        iter->iterable[iter->size++] = hs->ht[i]->key;
    }

    return iter;

}

void print_iterable(HS_Iterable* iter)
{
    printf("Printing iterable: ");
    if (iter->size == 0) 
    {
        printf("iter is empty");
        return;
    }

    printf("%d", iter->iterable[0]);

    for (int i = 1; i < iter->size; ++i)
    {
        printf(", %d", iter->iterable[i]);
    }

    printf("\n");
}

void hash_set_free(Hash_Set* hs)
{
    for (int i = 0; i < hs->total_size; ++i)
    {
        if (hs->ht[i]) free(hs->ht[i]);
    }
    free(hs->ht);
    free(hs);
}

void hash_set_iterable_free(HS_Iterable* iter)
{
    free(iter->iterable);
    free(iter);
}
