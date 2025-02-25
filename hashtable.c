#define HT_TEST

#include <memory.h>
#ifdef HT_TEST
#include <stdio.h>
#endif

#include "hashtable.h"


typedef struct ht_bucket {
    void *payload;
} ht_bucket;

uint8_t ht_init(ht *ht, ht_allocation_type alloc_type) {
    memset(ht, 0, sizeof(*ht));

    if (alloc_type == ALLOC_EAGER) {

    return 0;
}

#ifdef HT_TEST
int main() {
    printf("Hello World\n");
    return 0;
}
#endif
