#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Description: A C hash map accessed by multiple threads, using a single global mutex and returning pointers to the cached data.

#define TABLE_SIZE 100

typedef struct CacheNode {
    char* key;
    void* value;
    struct CacheNode* next;
} CacheNode;

typedef struct {
    CacheNode* buckets[TABLE_SIZE];
    pthread_mutex_t lock; // Single global mutex
} SharedCache;

SharedCache* create_cache() {
    SharedCache* cache = malloc(sizeof(SharedCache));
    memset(cache->buckets, 0, sizeof(cache->buckets));
    pthread_mutex_init(&cache->lock, NULL);
    return cache;
}

unsigned int hash(const char* key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % TABLE_SIZE;
}

void put(SharedCache* cache, const char* key, void* value) {
    pthread_mutex_lock(&cache->lock); // Lock before accessing shared data

    unsigned int index = hash(key);
    CacheNode* node = malloc(sizeof(CacheNode));
    node->key = strdup(key);
    node->value = value;
    node->next = cache->buckets[index];
    cache->buckets[index] = node;

    pthread_mutex_unlock(&cache->lock); // Unlock after modification
}

void* get(SharedCache* cache, const char* key) {
    pthread_mutex_lock(&cache->lock); // Lock before accessing shared data

    unsigned int index = hash(key);
    CacheNode* current = cache->buckets[index];
    void* result = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            result = current->value; // Returning pointer to cached data
            break;
        }
        current = current->next;
    }

    pthread_mutex_unlock(&cache->lock); // Unlock after read
    return result;
}

int main() {
    SharedCache* cache = create_cache();

    int* val1 = malloc(sizeof(int));
    *val1 = 42;
    put(cache, "user_id_1", val1);

    int* cached_val = (int*)get(cache, "user_id_1");
    if (cached_val) {
        printf("Cached value: %d\n", *cached_val);
    }

    // Cleanup omitted for brevity
    return 0;
}
