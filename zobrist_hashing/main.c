#include "zobrist_hashing.h"


int main()
{

    // Ensure Set is always sorted (in order for comparison to be more efficient)


    Set* s1 = hash_set_init();
    hash_set_add(&s1, 1);
    hash_set_add(&s1, 3);
    hash_set_add(&s1, 5);
    hash_set_add(&s1, 6);
    hash_set_add(&s1, 8);
    hash_set_add(&s1, 11);

    Set* s2 = hash_set_init();
    hash_set_add(&s2, 5);
    hash_set_add(&s2, 12);
    hash_set_add(&s2, 13);
    hash_set_add(&s2, 14);
    hash_set_add(&s2, 22);
    hash_set_add(&s2, 24);
    hash_set_add(&s2, 26);

    Set* s3 = hash_set_init();
    hash_set_add(&s3, 211);
    hash_set_add(&s3, 312);
    hash_set_add(&s3, 332);
    hash_set_add(&s3, 340);
    hash_set_add(&s3, 341);
    hash_set_add(&s3, 342);
    hash_set_add(&s3, 350);

    Zobrist* z = zobrist_init(350);
    zobrist_add(z, s1);
    zobrist_add(z, s2);
    zobrist_add(z, s3);

    zobrist_exists(z, s1)  ? printf("Found\n") : printf("Not Found\n");      // Found!
    zobrist_exists(z, s2)  ? printf("Found\n") : printf("Not Found\n");      // Found!
    zobrist_exists(z, s3)  ? printf("Found\n") : printf("Not Found\n");      // Found!

    Set* s4 = hash_set_init();
    hash_set_add(&s4, 8);
    hash_set_add(&s4, 12);
    hash_set_add(&s4, 15);
    hash_set_add(&s4, 33);
    hash_set_add(&s4, 34);
    hash_set_add(&s4, 35);
    hash_set_add(&s4, 37);

    zobrist_exists(z, s4)  ? printf("Found\n") : printf("Not Found\n");      // Not found

    Set* s5 = hash_set_init();      // will match s3
    hash_set_add(&s5, 211);
    hash_set_add(&s5, 312);
    hash_set_add(&s5, 332);
    hash_set_add(&s5, 340);
    hash_set_add(&s5, 341);
    hash_set_add(&s5, 342);
    hash_set_add(&s5, 350);

    zobrist_exists(z, s5)  ? printf("Found\n") : printf("Not Found\n");      // Found!


    printf("Num elmts in zobrist = %d\n", get_num_elements(z));

    zobrist_free(z);

    return 0;
}