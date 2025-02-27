#include <memory.h>
#include <stdlib.h>
#include <assert.h>

#include "hashtable.h"

#ifdef INTERNAL_HT_TEST
#include <stdio.h>
#endif

#define HT_INSERT_ELEMENT(heap, s, hv, v) { \
        static_assert(sizeof(*heap) == sizeof(struct ht), "Heap is not correct type"); \
        uint64_t _i; \
        for (_i = hv % s; (intptr_t)(heap->buckets[_i].payload) && !heap->buckets[_i].grave; _i = (_i + 1) % s); \
        heap->buckets[_i].payload = v; \
        }

typedef struct ht_bucket {
    void *payload;
    uint8_t grave;
} ht_bucket;

static inline uint8_t _ht_resize_table(ht * _Nonnull ht, uint64_t s) {
    uint64_t i;
    ht_bucket *b = ht->buckets;
    ht_bucket *curs;

    ht->buckets = calloc(sizeof(ht_bucket), s);
    if (ht->buckets == NULL) return 1;

    if (ht->n) {
        for (i = 0; i < ht->size; i++) {
            if (b[i].payload) {
                HT_INSERT_ELEMENT(ht, s, ht->hfn(b[i].payload), b[i].payload);
            }
        }
    }

    ht->size = s;

    return 0;
}

uint8_t ht_init(ht * _Nonnull ht, ht_hfn _Nonnull hfn, ht_allocation_type alloc_type) {
    memset(ht, 0, sizeof(*ht));

    if (alloc_type == ALLOC_EAGER) {
        if (_ht_resize_table(ht, HT_INITIAL_BUCKETS)) {
            return 1;
        }
    }
    
    assert(hfn);
    memcpy((void *) &ht->hfn, &hfn, sizeof(hfn));

    return 0;
}

uint8_t ht_insert(ht * _Nonnull ht, void * _Nonnull p) {
    if (ht->alloc_type == ALLOC_LAZY && (ht->size == 0 || ((ht->n * 10) / (ht->size)) > HT_MAX_LOAD_FACTOR)) {
        if (_ht_resize_table(ht, ht->size ? ht->size * 2 : HT_INITIAL_BUCKETS)) {
            return 1;
        }
    }

    HT_INSERT_ELEMENT(ht, ht->size, ht->hfn(p), p);
    ht->n++;

    if (ht->alloc_type == ALLOC_EAGER && ((ht->n * 10) / (ht->size)) >= HT_MAX_LOAD_FACTOR) {
        if (_ht_resize_table(ht, ht->size * 2)) {
            return 1;
        }
    }
    
    return 0;
}

void *ht_remove(ht * _Nonnull ht, void * _Nonnull p) {
    typeof (ht->hfn(p)) i, j;
    for (j = 0, i = (ht->hfn(p) % ht->size); ht->buckets[i].payload != p && ht->buckets[i].grave && j++ < ht->size; i = (i + 1) % ht->size);
    if (ht->buckets[i].payload != p) return NULL;
    ht->buckets[i].grave = 1;
    ht->n--;
    ht->buckets[i].payload = NULL;

    if ((ht->n * 10 / ht->size) < HT_MIN_LOAD_FACTOR && ht->size > HT_INITIAL_BUCKETS) {
        _ht_resize_table(ht, ht->size / 2);
    }

    return p;
}

uint8_t ht_destroy(ht * _Nonnull ht) {
    if (ht->buckets) free(ht->buckets);
    memset(ht, 0, sizeof(*ht));
    return 0;
}

#if defined(INTERNAL_HT_TEST) || defined(DEBUG)

static void _ht_default_print(const ht *ht, const void *p) {
    if (p == NULL) {
        printf("-");
    } else {
        printf("%p", p);
    }
}

void ht_print(ht *ht, ht_pprint _Nullable pprint) {
    typeof (ht->size) i;
    printf("Hashtable %p\n"
           "-------------------------\n"
           "Fill: %lu / %lu (Load Factor: %f)\n"
           "Allocation Type: %s\n"
           "-------------------------\n"
           "Element # - Value\n" ,
           ht,
           ht->n,
           ht->size,
           ht->size ? ((double) ht->n) * 10 / ht->size : 0,
           ht_alloc_types[ht->alloc_type]
    );

    if (pprint == NULL) pprint = _ht_default_print;
    for (i = 0; i < ht->size; i++) {
        printf("%lu - ", i);
        pprint(ht, ht->buckets[i].payload);
        printf("\n");
    }

    printf("-------------------------\n");
}

#endif

#ifdef INTERNAL_HT_TEST
uint64_t test_hash(const void *p) { return (uint64_t) p; }
int main() {
    uint32_t i;
    ht h;

    assert(!ht_init(&h, test_hash, ALLOC_EAGER));
    ht_print(&h, NULL);

    for (i = 0; i < 8; i++) {
        assert(!ht_insert(&h, (void *) 1));
        ht_print(&h, NULL);
    }

    for (i = 0; i < 6; i++) {
        assert(ht_remove(&h, (void *) 1));
        ht_print(&h, NULL);
    }

    assert(!ht_destroy(&h));

    return 0;
}
#endif
