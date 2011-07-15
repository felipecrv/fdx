#include <assert.h>
#include <stdio.h>
#include "../assembler/hashmap.h"

#define xassert(STR) putchar('.');assert(STR);

struct hashmap h;
int foo = 5, bar = 7, *p_foo, *p_bar;

int main()
{
    printf("Running tests:\n");

    xassert(hashmap_init(&h, 5));

    xassert(hashmap_hash(&h, "foo") == 3);
    xassert(hashmap_hash(&h, "foo") == 3);
    xassert(hashmap_hash(&h, "foo") == 3);
    xassert(hashmap_hash(&h, "bar") == 3);
    xassert(hashmap_hash(&h, "bar") == 3);

    // collision detection
    xassert(hashmap_set(&h, "foo", (void*) &foo));
    xassert(hashmap_set(&h, "bar", (void*) &bar));
    p_foo = (int *) hashmap_lookup(&h, "foo");
    p_bar = (int *) hashmap_lookup(&h, "bar");
    xassert(*p_foo == foo);
    xassert(*p_bar == bar);

    // overriding
    xassert(hashmap_set(&h, "bar", (void*) &foo));
    p_bar = (int *) hashmap_lookup(&h, "bar");
    xassert(*p_foo == foo);
    xassert(*p_bar == foo);

    hashmap_destroy(&h);

    // full hash table
    xassert(hashmap_init(&h, 2));

    xassert(hashmap_set(&h, "foo", (void*) &foo));
    xassert(hashmap_set(&h, "bar", (void*) &bar));
    p_foo = (int *) hashmap_lookup(&h, "foo");
    p_bar = (int *) hashmap_lookup(&h, "bar");
    xassert(*p_foo == foo);
    xassert(*p_bar == bar);

    xassert(!hashmap_set(&h, "a key", (void *) &foo));

    putchar('\n');

    return 0;
}
