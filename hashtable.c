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

#define LFAC(h) (((double) (h)->n * 10) / (h)->size)

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

    if (b) free(b);

    return 0;
}

uint8_t ht_init(ht * _Nonnull ht, ht_hfn _Nonnull hfn, ht_allocation_type alloc_type) {
    memset(ht, 0, sizeof(*ht));
    ht->alloc_type = alloc_type;

    if (alloc_type == ALLOC_EAGER) {
        if (_ht_resize_table(ht, HT_INITIAL_BUCKETS)) {
            return 1;
        }
    }
    
    assert(hfn);
    ht->hfn = hfn;

    return 0;
}

uint8_t ht_insert(ht * _Nonnull ht, void * _Nonnull p) {
    if (ht->alloc_type == ALLOC_LAZY && (ht->size == 0 || LFAC(ht) > HT_MAX_LOAD_FACTOR)) {
        if (_ht_resize_table(ht, ht->size ? ht->size * 2 : HT_INITIAL_BUCKETS)) {
            return 1;
        }
    }

    HT_INSERT_ELEMENT(ht, ht->size, ht->hfn(p), p);
    ht->n++;

    if (ht->alloc_type == ALLOC_EAGER && LFAC(ht) >= HT_MAX_LOAD_FACTOR) {
        if (_ht_resize_table(ht, ht->size * 2)) {
            return 1;
        }
    }
    
    return 0;
}

void *ht_remove(ht * _Nonnull ht, void * _Nonnull p) {
    typeof (ht->hfn(p)) i, j;
    for (j = 0, i = (ht->hfn(p) % ht->size); ht->buckets[i].payload != p && (ht->buckets[i].payload || ht->buckets[i].grave) && j++ < ht->size; i = (i + 1) % ht->size);
    if (ht->buckets[i].payload != p) return NULL;
    ht->buckets[i].grave = 1;
    ht->n--;
    ht->buckets[i].payload = NULL;

    if (ht->size > HT_INITIAL_BUCKETS && LFAC(ht) < HT_MIN_LOAD_FACTOR) {
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
    p == NULL ? printf("-") : printf("%p", p);
}

void ht_print(ht *ht, ht_pprint _Nullable pprint) {
    typeof (ht->size) i;
    printf("Hashtable %p\n"
           "------------------------------------------\n"
           "Fill: %lu / %lu (Load Factor: %f)\n"
           "Allocation Type: %s\n"
           "------------------------------------------\n",
           ht,
           ht->n,
           ht->size,
           ht->size ? LFAC(ht) : 0,
           ht_alloc_types[ht->alloc_type]
    );

    if (ht->size < 1024) {
        printf("Element # - Value\n");
        if (pprint == NULL) pprint = _ht_default_print;
        for (i = 0; i < ht->size; i++) {
            printf("%lu - ", i);
            ht->buckets[i].grave ? printf("🪦") : pprint(ht, ht->buckets[i].payload);
            printf("\n");
        }
    } else {
        printf("-- Omitting Elements, too many to print --\n");
    }

    printf("------------------------------------------\n");
}

#endif

#ifdef INTERNAL_HT_TEST
uint64_t test_hash(const void *p) { return (uint64_t) p; }
int main() {
    ht h;
    uint32_t k, i, n = 1280000;
    uint64_t *arr = NULL, res;

    if (!(arr = malloc(n * sizeof(*arr)))) {
        return -1;
    }

    arc4random_buf(arr, n * sizeof(*arr));

    assert(!ht_init(&h, test_hash, ALLOC_EAGER));
    assert(h.size == HT_INITIAL_BUCKETS);

    for (i = 0; i < n; i++) {
        ht_insert(&h, (void *) arr[i]);
        assert(h.size > 0 && LFAC(&h) < HT_MAX_LOAD_FACTOR);
    }

    ht_print(&h, NULL);

    for (i = 0; i < n; i++) {
        if (arr[i] != (res = (typeof(arr[i])) ht_remove(&h, (void *) arr[i]))) {
            printf("\033[31mERROR REMOVING %lx, got %lx, i = %d\n", arr[i], res, i);
            ht_print(&h, NULL);
            printf("\033[0m");
            assert(0);
        }

        assert(h.size == HT_INITIAL_BUCKETS || LFAC(&h) > HT_MIN_LOAD_FACTOR);
    }

    ht_print(&h, NULL);
    assert(!ht_destroy(&h));

    assert(!ht_init(&h, test_hash, ALLOC_LAZY));
    assert(h.size == 0);

    k = 0;
    for (i = 0; i < n; i++) {
        ht_insert(&h, (void *) arr[i]);
        assert((h.size > 0 && LFAC(&h) <= HT_MAX_LOAD_FACTOR && !(k = 0)) || k++ < 1);
    }

    ht_print(&h, NULL);

    k = 0;
    for (i = 0; i < n; i++) {
        if (arr[i] != (res = (typeof(arr[i])) ht_remove(&h, (void *) arr[i]))) {
            printf("\033[31mERROR REMOVING %lx, got %lx, i = %d\n", arr[i], res, i);
            ht_print(&h, NULL);
            printf("\033[0m");
            assert(0);
        }

        assert(h.size == 0 || h.size == HT_INITIAL_BUCKETS || ((LFAC(&h) > HT_MIN_LOAD_FACTOR && !(k = 0)) || k++ < 1));
    }

    ht_print(&h, NULL);
    assert(!ht_destroy(&h));

    free(arr);


    return 0;
}
#endif
