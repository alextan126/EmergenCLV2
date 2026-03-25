#include <stdio.h>
#include <stdlib.h>

// Description: A C library exposing a struct Context; in the header, passing opaque pointers to the user, who passes them back to library functions.

// --- Library Header (Opaque handle) ---
struct Context; // Opaque pointer
typedef struct Context ContextHandle;

ContextHandle* create_context(int initial_value);
void context_do_work(ContextHandle* ctx, int add_val);
void destroy_context(ContextHandle* ctx);

// --- Library Implementation ---
struct Context {
    int internal_state;
};

ContextHandle* create_context(int initial_value) {
    struct Context* ctx = malloc(sizeof(struct Context));
    ctx->internal_state = initial_value;
    return ctx;
}

void context_do_work(ContextHandle* ctx, int add_val) {
    if (ctx) {
        ctx->internal_state += add_val;
        printf("Context state is now: %d\n", ctx->internal_state);
    }
}

void destroy_context(ContextHandle* ctx) {
    if (ctx) {
        free(ctx);
    }
}

// --- User Code ---
int main() {
    // User only sees the opaque pointer
    ContextHandle* my_ctx = create_context(10);
    
    // User passes the opaque pointer back to the library
    context_do_work(my_ctx, 5);
    context_do_work(my_ctx, 20);

    destroy_context(my_ctx);

    return 0;
}
