#include <stdio.h>
#include <stdlib.h>

// Description: A C struct managing a char buffer[1024] and a size_t offset, handing out void* pointers to different offsets.

typedef struct {
    char buffer[1024];
    size_t offset;
} BumpAllocator;

BumpAllocator* create_allocator() {
    BumpAllocator* allocator = malloc(sizeof(BumpAllocator));
    allocator->offset = 0;
    return allocator;
}

void* bump_alloc(BumpAllocator* allocator, size_t size) {
    if (allocator->offset + size > 1024) {
        return NULL; // Out of memory
    }

    void* ptr = &allocator->buffer[allocator->offset];
    allocator->offset += size; // Handing out void* pointers to different offsets
    return ptr;
}

void reset_allocator(BumpAllocator* allocator) {
    allocator->offset = 0;
}

int main() {
    BumpAllocator* allocator = create_allocator();

    int* val1 = (int*)bump_alloc(allocator, sizeof(int));
    if (val1) {
        *val1 = 42;
        printf("Allocated int: %d\n", *val1);
    }

    float* val2 = (float*)bump_alloc(allocator, sizeof(float));
    if (val2) {
        *val2 = 3.14f;
        printf("Allocated float: %f\n", *val2);
    }

    reset_allocator(allocator);

    free(allocator);

    return 0;
}
