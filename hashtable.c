#define HT_TEST

#include <memory.h>
#include <stdlib.h>
#ifdef HT_TEST
#include <stdio.h>
#endif

#include "hashtable.h"

#define HT_INSERT_ELEMENT(heap, hv, v) ({ \
        assert(sizeof(*heap) == sizeof(struct ht)); \
        uint64_t i; \
        for (i = hv; heap->buckets[i] && !heap->buckets[i].grave; i = (i + 1) % heap->size); \
        heap->buckets[i].payload = v; \
        }) \

typedef struct ht_bucket {
    void *payload;
    uint8_t grave;
} ht_bucket;

uint8_t ht_init(ht *ht, ht_allocation_type alloc_type) {
    memset(ht, 0, sizeof(*ht));

    if (alloc_type == ALLOC_EAGER) {
        ht->buckets = calloc(HT_INITIAL_BUCKETS, sizeof(ht_bucket));
        if (ht->buckets == NULL) {
            return 1;
        }
    }

    return 0;
}

static inline uint8_t _ht_resize_table(ht *ht, uint64_t s) {
    uint64_t i;
    ht_bucket *b = ht->buckets;
    ht_bucket *curs;

    ht->buckets = calloc(sizeof(ht_bucket), s);
    if (ht->buckets == NULL) return 0;

    if (ht->n) {
        curs = ht->buckets + ht->size;
        do {
            curs = curs - 1;
            if (curs) {

uint8_t ht_insert(ht *ht, void *p) {
    if (ht->alloc_type == ALLOC_LAZY && (ht->size == 0 || ((ht->n * 10) / (ht->size)) > HT_MAX_LOAD_FACTOR)) {

    }
}

#ifdef HT_TEST
int main() {
    printf("Hello World\n");
    return 0;
}
#endif
