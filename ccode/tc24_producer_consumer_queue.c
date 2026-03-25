#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Description: A C queue using condition variables (pthread_cond_t) and manual locking to push/pop tasks.

#define QUEUE_CAPACITY 10

typedef struct {
    int id;
} Task;

typedef struct {
    Task* tasks[QUEUE_CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} TaskQueue;

TaskQueue* create_queue() {
    TaskQueue* queue = malloc(sizeof(TaskQueue));
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
    return queue;
}

void push_task(TaskQueue* queue, Task* task) {
    pthread_mutex_lock(&queue->lock); // Manual locking

    while (queue->count == QUEUE_CAPACITY) {
        pthread_cond_wait(&queue->not_full, &queue->lock); // Condition variable
    }

    queue->tasks[queue->tail] = task;
    queue->tail = (queue->tail + 1) % QUEUE_CAPACITY;
    queue->count++;

    pthread_cond_signal(&queue->not_empty); // Signal consumer
    pthread_mutex_unlock(&queue->lock); // Manual unlocking
}

Task* pop_task(TaskQueue* queue) {
    pthread_mutex_lock(&queue->lock); // Manual locking

    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock); // Condition variable
    }

    Task* task = queue->tasks[queue->head];
    queue->head = (queue->head + 1) % QUEUE_CAPACITY;
    queue->count--;

    pthread_cond_signal(&queue->not_full); // Signal producer
    pthread_mutex_unlock(&queue->lock); // Manual unlocking

    return task;
}

void* producer_thread(void* arg) {
    TaskQueue* queue = (TaskQueue*)arg;
    for (int i = 0; i < 5; i++) {
        Task* task = malloc(sizeof(Task));
        task->id = i;
        printf("Producing task %d\n", task->id);
        push_task(queue, task);
        sleep(1);
    }
    return NULL;
}

void* consumer_thread(void* arg) {
    TaskQueue* queue = (TaskQueue*)arg;
    for (int i = 0; i < 5; i++) {
        Task* task = pop_task(queue);
        printf("Consuming task %d\n", task->id);
        free(task);
    }
    return NULL;
}

int main() {
    TaskQueue* queue = create_queue();
    pthread_t producer, consumer;

    pthread_create(&producer, NULL, producer_thread, queue);
    pthread_create(&consumer, NULL, consumer_thread, queue);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    return 0;
}
