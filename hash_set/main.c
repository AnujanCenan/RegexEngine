#include "include/primitive.h"

int main()
{
    Hash_Set* set = hash_set_init();
    hash_set_add(&set, 14);
    hash_set_add(&set, 54);
    hash_set_add(&set, 1234);
    hash_set_add(&set, 5312);
    hash_set_add(&set, 3432);
    hash_set_add(&set, 980);
    hash_set_add(&set, 5342);
    hash_set_add(&set, 1234);
    hash_set_add(&set, 623424);
    hash_set_add(&set, 21342123);
    hash_set_add(&set, 62);
    hash_set_add(&set, 3214);

    printf("exists(1234) = %d\n", hash_set_exists(set, 1234));
    printf("exists(5312) = %d\n", hash_set_exists(set, 5312));
    printf("exists(62) = %d\n", hash_set_exists(set, 62));
    printf("exists(980) = %d\n", hash_set_exists(set, 980));


    printf("exists(1) = %d\n", hash_set_exists(set, 1));
    printf("exists(22) = %d\n", hash_set_exists(set, 22));
    printf("exists(43123) = %d\n", hash_set_exists(set, 43123));
    printf("exists(99999) = %d\n", hash_set_exists(set, 99999));


    printf("Before deletion: exists(54) = %d\n", hash_set_exists(set, 54));
    hash_set_delete(set, 54);
    printf("After deletion: exists(54) = %d\n", hash_set_exists(set, 54));

    HS_Iterable* iter = hash_set_get_iterable(set);
    int sum = 0;
    for (int i = 0; i < iter->size; ++i)
    {
        sum += iter->iterable[i];
    }

    printf("Sum = %d\n", sum);      // should be 21985137 (need to exclude duplicate 1234 and deleted 54)

    hash_set_free(set);
    iterable_free(iter);
    
    return 0;
}