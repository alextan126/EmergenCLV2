#include <stdio.h>
#include <stdlib.h>

// Description: A C array holding different structs using a tagged union and void* payloads.

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
} DataType;

typedef struct {
    int value;
} IntStruct;

typedef struct {
    float value;
} FloatStruct;

typedef struct {
    char* value;
} StringStruct;

typedef struct {
    DataType type;
    union {
        void* payload; // void* payload holding different structs
    } data;
} HeterogeneousElement;

int main() {
    HeterogeneousElement array[3];

    IntStruct* i_struct = malloc(sizeof(IntStruct));
    i_struct->value = 42;
    array[0].type = TYPE_INT;
    array[0].data.payload = i_struct;

    FloatStruct* f_struct = malloc(sizeof(FloatStruct));
    f_struct->value = 3.14f;
    array[1].type = TYPE_FLOAT;
    array[1].data.payload = f_struct;

    StringStruct* s_struct = malloc(sizeof(StringStruct));
    s_struct->value = "Hello World";
    array[2].type = TYPE_STRING;
    array[2].data.payload = s_struct;

    for (int i = 0; i < 3; i++) {
        switch (array[i].type) {
            case TYPE_INT:
                printf("Int: %d\n", ((IntStruct*)array[i].data.payload)->value);
                break;
            case TYPE_FLOAT:
                printf("Float: %f\n", ((FloatStruct*)array[i].data.payload)->value);
                break;
            case TYPE_STRING:
                printf("String: %s\n", ((StringStruct*)array[i].data.payload)->value);
                break;
        }
    }

    free(i_struct);
    free(f_struct);
    free(s_struct);

    return 0;
}
