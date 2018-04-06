#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct InputBuffer_t
{
        char* buffer;
        size_t buffer_length;
        ssize_t input_length;
};

// Create type of InputBuffer_t and named it InputBuffer
typedef struct InputBuffer_t InputBuffer;

enum MetaCommandResult_t {
        META_COMMAND_SUCCESS,
        META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetacomandResult;

enum Prepareresult_t {
        PREPARE_SUCCESS,
        PREPARE_UNRECOGNIZED_STATMENT
};
typedef enum Prepareresult_t PrepareResult;

enum StatmentType_t {
        STATMENT_INSERT,
        STATMENT_SELECT
};
typedef enum StatmentType_t StatementType;

struct Statment_t {
        StatementType type;
};
typedef struct Statment_t Statment;

InputBuffer* new_input_buffer()
{
        //alllocate memmory for input
        InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
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
        ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

        if (bytes_read <= 0) {
                printf("Error reading input\n");
                exit(EXIT_FAILURE);
        }

        input_buffer->input_length = bytes_read - 1;
        input_buffer->buffer[bytes_read - 1] = 0;
}

MetacomandResult do_meta_command(InputBuffer* input_buffer) {
        if (strcmp(input_buffer->buffer, ".exit") == 0) {
                exit(EXIT_SUCCESS);
        } else {
                return META_COMMAND_UNRECOGNIZED_COMMAND;
        }
}

PrepareResult prepare_statment(InputBuffer* input_buffer, Statment* statment) {
        if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
                statment->type = STATMENT_INSERT;
                return PREPARE_SUCCESS;
        }
        if (strcmp(input_buffer->buffer, "select") == 0) {
                statment->type = STATMENT_SELECT;
                return PREPARE_SUCCESS;
        }

        return PREPARE_UNRECOGNIZED_STATMENT;
}

void execute_statment(Statment* statement) {
        switch (statement->type) {
        case(STATMENT_INSERT):
                printf("this is where we would do an insert.\n");
                break;
        case (STATMENT_SELECT):
                printf("This is where we would do a select.\n");
                break;
        }
        
}

int main(int argc, char* argv[])
{
        InputBuffer* input_buffer = new_input_buffer();
        while (true) {
                print_prompt();
                read_input(input_buffer);
                if (input_buffer->buffer[0] == '.') {
                        switch (do_meta_command(input_buffer)) {
                        case (META_COMMAND_SUCCESS):
                                continue;
                        case (META_COMMAND_UNRECOGNIZED_COMMAND):
                                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                                continue;
                        }
                }
                Statment statment;
                switch (prepare_statment(input_buffer, &statment)) {
                case (PREPARE_SUCCESS):
                        break;
                case (PREPARE_UNRECOGNIZED_STATMENT):
                        printf("Unrecognized keyword at start of '%s'\n",
                               input_buffer->buffer);
                        continue;
                }
                execute_statment(&statment);
                printf("Executed.\n");
        }
}
