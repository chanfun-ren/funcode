#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    char *buffer;
    size_t buffer_len;
    ssize_t input_len;
} InputBuffer;

InputBuffer *new_input_buffer() {
    InputBuffer *pbuffer = (InputBuffer*)malloc(sizeof(InputBuffer));
    pbuffer->buffer = NULL;
    pbuffer->buffer_len = 0;
    pbuffer->input_len = 0;
    return pbuffer;
}
void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void print_prompt() { 
    printf("db > ");
}

void read_input(InputBuffer *input_buffer) {
    size_t read_bytes = getline(&(input_buffer->buffer), &(input_buffer->buffer_len), stdin);
    if (read_bytes <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
    input_buffer->input_len = read_bytes - 1;
    input_buffer->buffer[read_bytes - 1] = '\0';
}

int main() {
    InputBuffer *input_buffer = new_input_buffer();
    while (true) {
        print_prompt();
        read_input(input_buffer);

        if (strcmp(input_buffer->buffer, ".exit") == 0) {
            close_input_buffer(input_buffer);
            exit(EXIT_SUCCESS);
        } else {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);
        }
    }
    return 0;
}