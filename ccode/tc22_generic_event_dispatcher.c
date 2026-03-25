#include <stdio.h>
#include <stdlib.h>

// Description: A C publisher/subscriber model where callbacks are registered as void (*func)(void* user_data) along with a void* context pointer.

typedef void (*CallbackFunc)(void* user_data);

typedef struct {
    CallbackFunc func;
    void* user_data;
} Subscriber;

typedef struct {
    Subscriber* subscribers;
    size_t count;
    size_t capacity;
} EventDispatcher;

EventDispatcher* create_dispatcher() {
    EventDispatcher* dispatcher = malloc(sizeof(EventDispatcher));
    dispatcher->capacity = 10;
    dispatcher->count = 0;
    dispatcher->subscribers = malloc(sizeof(Subscriber) * dispatcher->capacity);
    return dispatcher;
}

void register_callback(EventDispatcher* dispatcher, CallbackFunc func, void* user_data) {
    if (dispatcher->count < dispatcher->capacity) {
        dispatcher->subscribers[dispatcher->count].func = func;
        dispatcher->subscribers[dispatcher->count].user_data = user_data;
        dispatcher->count++;
    }
}

void dispatch_event(EventDispatcher* dispatcher) {
    for (size_t i = 0; i < dispatcher->count; i++) {
        // Call the registered function with its context pointer
        dispatcher->subscribers[i].func(dispatcher->subscribers[i].user_data);
    }
}

// Example usage
void my_callback(void* user_data) {
    int* val = (int*)user_data;
    printf("Callback triggered with data: %d\n", *val);
}

int main() {
    EventDispatcher* dispatcher = create_dispatcher();
    
    int my_data_1 = 100;
    int my_data_2 = 200;

    register_callback(dispatcher, my_callback, &my_data_1);
    register_callback(dispatcher, my_callback, &my_data_2);

    dispatch_event(dispatcher);

    free(dispatcher->subscribers);
    free(dispatcher);

    return 0;
}
