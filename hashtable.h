#ifndef H_HASHTABLE_5
#define H_HASHTABLE_5

#include <inttypes.h>

#define HT_INITIAL_BUCKETS 10
#define HT_MAX_LOAD_FACTOR 7
#define HT_MIN_LOAD_FACTOR 3

typedef struct ht_bucket ht_bucket;
typedef uint64_t (*ht_hfn)(void *);

typedef enum {
    ALLOC_EAGER,
    ALLOC_LAZY,
} ht_allocation_type;

typedef struct ht {
    const ht_hfn hfn;
    ht_bucket *buckets;
    ht_allocation_type alloc_type;

    uint64_t size;
    uint64_t n;
} ht;

#endif
