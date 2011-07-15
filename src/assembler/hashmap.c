#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

struct hashmap* hashmap_init(struct hashmap* h, size_t nmemb)
{
    h->nmemb = nmemb;
    h->table = (struct hashmap_node*)
        calloc(nmemb, sizeof(struct hashmap_node));
    if (!h->table)
        return NULL;
    return h;
}

size_t hashmap_hash(struct hashmap* h, char* key)
{
    size_t i, len, hash = 0;
    
    len = strlen(key);
    for(i = 0; i < len; i++) {
        hash += (key[i] - 'A') * 37;
    }
    return hash % h->nmemb;
}

void* hashmap_set(struct hashmap* h, char* key, void* value)
{
    size_t hash = hashmap_hash(h, key);
    size_t i, j;

    for (i = hash; i < h->nmemb + hash; i++) {
        j = (i >= h->nmemb) ? i - h->nmemb : i;
        
        if (!h->table[j].key || strcmp(h->table[j].key, key) == 0) {
            h->table[j].key = (char *) malloc((strlen(key) + 1) * sizeof(char));
            h->table[j].value = value;
            strcpy(h->table[j].key, key);
            return value;
        }
    }
    return NULL;
}

void* hashmap_lookup(struct hashmap *h, char* key)
{
    size_t hash = hashmap_hash(h, key);
    size_t i, j;

    for (i = hash; i < h->nmemb + hash; i++) {
        j = (i >= h->nmemb) ? i - h->nmemb : i;
                
        if (!h->table[j].key) {
            return NULL;
        }
        if (strcmp(key, h->table[j].key) == 0) {
            return h->table[j].value;
        }
    }
    return NULL;
}

void hashmap_destroy(struct hashmap* h)
{
    int i;
    for (i = 0; i < h->nmemb; i++) {
        if (h->table[i].key)
            free(h->table[i].key);
    }
    free(h->table);
}
