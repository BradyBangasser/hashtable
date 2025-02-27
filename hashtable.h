#ifndef H_HASHTABLE_5
#define H_HASHTABLE_5

#include <inttypes.h>

#if defined(__clang__) || __has_feature(nullability)
#define _Nonnull _Nonnull
#define _Nullable _Nullable
#else
#define _Nonnull
#define _Nullable
#endif

#define INTERNAL_HT_TEST

#define HT_INITIAL_BUCKETS 8
#define HT_MAX_LOAD_FACTOR 7
#define HT_MIN_LOAD_FACTOR 3

typedef struct ht ht;
typedef struct ht_bucket ht_bucket;
typedef uint64_t (*ht_hfn)(const void * _Nonnull);
typedef void (*ht_pprint)(const ht * _Nonnull, const void * _Nullable);

#define ALLOCATION_TYPES(F) \
    F(ALLOC_EAGER) \
    F(ALLOC_LAZY)

#define ENUM(E) E,
#define STR(E) #E,

typedef enum {
    ALLOCATION_TYPES(ENUM)
} ht_allocation_type;

#if defined(INTERNAL_HT_TEST) || defined (DEBUG)
static const char * _Nonnull ht_alloc_types[] = { ALLOCATION_TYPES(STR) };
#endif

#undef ENUM
#undef STR
#undef ALLOCATION_TYPES

typedef struct ht {
    const ht_hfn _Nullable hfn;
    ht_bucket * _Nullable buckets;
    ht_allocation_type alloc_type;

    uint64_t size;
    uint64_t n;
} ht;

uint8_t ht_init(ht * _Nonnull ht, ht_hfn _Nonnull hfn, ht_allocation_type alloc_type);
uint8_t ht_insert(ht * _Nonnull ht, void * _Nonnull p);
void *ht_remove(ht * _Nonnull ht, void * _Nonnull p);
uint8_t ht_destroy(ht * _Nonnull ht);
void ht_print(ht *ht, ht_pprint _Nullable pprint);
#endif
