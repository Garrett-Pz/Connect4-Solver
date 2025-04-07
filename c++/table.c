#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

// Bound is required becuase alpha-beta pruing does not necessarily give
// exact result.
// -1: lower bound (ans is >= stored value)
// 1 : upper bound (<=)
struct Entry {
    uint64_t p1;
    uint64_t p2;
    int result;
    int bound;
};

int TABLE_SIZE;

void table_init();
void table_reset(struct Entry**);
void table_free(struct Entry**);
int table_hash(uint64_t, uint64_t);
void table_put(struct Entry**, int, uint64_t, uint64_t, int, int);
void table_put_t(struct Entry**, uint64_t, uint64_t, int, int, double, int);
struct Entry* table_get(struct Entry**, uint64_t, uint64_t);
int table_contains(struct Entry**, uint64_t, uint64_t, int);

// 1048573
void table_init() {
    TABLE_SIZE = 1 << 27;
}

void table_reset(struct Entry** T) {
    for(uint64_t i = 0; i < TABLE_SIZE; i++) {
        if(T[i] != NULL) {
            free(T[i]);
            T[i] = NULL;
        }
    }
}

void table_free(struct Entry** T) {
    for(uint64_t i = 0; i < TABLE_SIZE; i++) {
        free(T[i]);
    }

    free(T);
}

int table_hash(uint64_t p1, uint64_t p2) {
    uint64_t hash = 0;

    hash += p1;
    hash += hash << 20;
    hash ^= hash >> 13;

    hash += p2;
    hash += hash << 20;
    hash ^= hash >> 13;

    /*
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    */

    hash += p1;
    hash += hash >> 20;
    hash ^= hash << 13;

    hash += p2;
    hash += hash >> 20;
    hash ^= hash << 13;

    /*
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    */

    return hash % TABLE_SIZE;
}

void table_put(struct Entry** T, int hash, uint64_t p1, uint64_t p2, int value, int b) {
    //int hash = table_hash(p1, p2);
    struct Entry *newEntry, *oldEntry;

    oldEntry = T[hash];
    if(oldEntry != 0) {
        //printf("your hash sucks\n");
        free(oldEntry);
    }

    newEntry = (struct Entry*) calloc(1, sizeof(struct Entry));
    newEntry->p1 = p1;
    newEntry->p2 = p2;
    newEntry->result = value;
    newEntry->bound = b;
    T[hash] = newEntry;
}

/*
void table_put_t(struct Entry** T, uint64_t p1, uint64_t p2, int value, int b, double t, int p) {
    int hash = table_hash(p1, p2);
    struct Entry *newEntry, *oldEntry;

    oldEntry = T[hash];
    if(oldEntry != 0) {
        //printf("your hash sucks\n");
        
        if(oldEntry->time > 3*t && (p - oldEntry->pos) < (2 << 15)) {
            return;
        }
        
        free(oldEntry);
    }

    newEntry = (struct Entry*) calloc(1, sizeof(struct Entry));
    newEntry->p1 = p1;
    newEntry->p2 = p2;
    newEntry->result = value;
    newEntry->bound = b;
    newEntry->time = t;
    newEntry->pos = p;
    T[hash] = newEntry;
}
*/

struct Entry* table_get(struct Entry** T, uint64_t p1, uint64_t p2) {
    return T[table_hash(p1, p2)];
}

/*
int table_contains(struct Entry** T, uint64_t p1, uint64_t p2) {
    struct Entry* entry = T[table_hash(p1, p2)];
    if(entry == 0) return 0;
    if(entry->p1 == p1 && entry->p2 == p2) {
        return 1;
    }
    else {
        return 0;
    }
}
*/

int table_contains(struct Entry** T, uint64_t p1, uint64_t p2, int hash) {
    struct Entry* entry = T[hash];
    if(entry == 0) return 0;
    if(entry->p1 == p1 && entry->p2 == p2) {
        return 1;
    }
    else {
        return 0;
    }
}