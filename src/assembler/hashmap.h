#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stdlib.h>

struct hashmap_node {
    char* key;
    void* value;
};

struct hashmap {
    struct hashmap_node* table;
    size_t nmemb;
};

struct hashmap* hashmap_init(struct hashmap*, size_t);
size_t hashmap_hash(struct hashmap*, char*);
void* hashmap_lookup(struct hashmap*, char*);
void* hashmap_set(struct hashmap*, char*, void*);
void hashmap_destroy(struct hashmap* );

#endif /*HASHMAP_H_*/
