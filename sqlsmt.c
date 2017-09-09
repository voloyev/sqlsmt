#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Here init structture
// buffer with char pointer
// As an implication, size_t is a type guaranteed to hold any array index : buffer_length
//
// int can store values of at least the range [-2^15 ... 2^15-1] per ISO C
// ssize_t can store values of at least the range [-1 ... 2^15-1] per POSIX (see _POSIX_SSIZE_MAX).
// its about input_length
struct InputBuffer_t
{
        char* buffer;
        size_t buffer_length;
        ssize_t input_length;
};

// Create type of InputBuffer_t and named it InputBuffer
typedef struct InputBuffer_t InputBuffer;
// Create function with InputBuffer type
InputBuffer* new_input_buffer()
{
        //alllocate memmory for input
        InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
        // foo->bar is equivalent to (*foo).bar, i.e. it gets the member called bar from the struct that foo points to.
        // assign fields of InputBuffer_t to some values
        input_buffer->buffer = NULL;
        input_buffer->buffer_length = 0;
        input_buffer->input_length = 0;

        return input_buffer;
}

void print_prompt()
{
        //create print promt text
        printf("sqlsmt > ");
}

void read_input(InputBuffer* input_buffer)
{
        // &a; returns the actual address of the variable.
        /*
         * Here’s a typical getline() statement:
         *
         *          getline(&buffer,&size,stdin);
         *
         *          The getline() function is prototyped in the stdio.h header file. Here are the three arguments:
         *
         *          &buffer is the address of the first character position where the input string will be stored.
         *          It’s not the base address of the buffer, but of the first character in the buffer.
         *           This pointer type (a pointer-pointer or the ** thing) causes massive confusion.
         *
         *          &size is the address of the variable that holds the size of the input buffer, another pointer.
         *
         *          stdin is the input file handle. So you could use getline() to read a line of text from a file,
         *           but when stdin is specified, standard input is read.
         */
        ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

        if (bytes_read <= 0) {
                printf("Error reading input\n");
                exit(EXIT_FAILURE);
        }

        // Ignore trailing newline
        // remove newline 
        input_buffer->input_length = bytes_read - 1;
        // add 0 at the end of a buffer
        input_buffer->buffer[bytes_read - 1] = 0;
}

int main(int argc, char* argv[])
{
        InputBuffer* input_buffer = new_input_buffer();
        while (true) {
                print_prompt();
                read_input(input_buffer);

                if (strcmp(input_buffer->buffer, ".exit") == 0) {
                        // when command .exit than exit the buffer
                        puts("Bye!");
                        exit(EXIT_SUCCESS);
                } else {
                        printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                }
        }
}
