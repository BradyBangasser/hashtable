#ifndef H_HASHTABLE_5
#define H_HASHTABLE_5

#include <inttypes.h>

typedef struct ht_bucket ht_bucket;
typedef uint64_t (*ht_hfn)(void *);

typedef struct ht {
    const ht_hfn hfn;
    ht_bucket *buckets;

    uint64_t size;
    uint64_t n;
} ht;

typedef enum __attribute__((packed)) {
    ALLOC_EAGER,
    ALLOC_LAZY,
} ht_allocation_type;

#endif
