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

enum ExecuteResult_t {
        EXECUTE_SUCCESS,
        EXECUTE_TABLE_FULL
};
typedef enum ExecuteResult_t ExecuteResult;

enum MetaCommandResult_t {
        META_COMMAND_SUCCESS,
        META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetacomandResult;

enum Prepareresult_t {
        PREPARE_SUCCESS,
        PREPARE_SYNTAX_ERROR,
        PREPARE_UNRECOGNIZED_STATMENT
};
typedef enum Prepareresult_t PrepareResult;

enum StatmentType_t {
        STATMENT_INSERT,
        STATMENT_SELECT
};
typedef enum StatmentType_t StatementType;

const uint32_t COLUMN_USERNAME_SIZE = 32;
const uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row_t {
        uint32_t id;
        char username[COLUMN_USERNAME_SIZE];
        char email[COLUMN_EMAIL_SIZE];
};
typedef struct Row_t Row;

struct Statment_t {
        StatementType type;
        Row row_to_insert;
};
typedef struct Statment_t Statment;
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t TABLE_MAX_PAGES = 100;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table_t {
        void* pages[TABLE_MAX_PAGES];
        uint32_t num_rows;
};
typedef struct Table_t Table;

void print_row(Row* row) {
        printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

void serialize_row(Row* source, void* destination) {
        memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
        memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
        memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination) {
        memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
        memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
        memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_slot(Table* table, uint32_t row_num) {
        uint32_t page_num = row_num / ROWS_PER_PAGE;
        void* page = table->pages[page_num];
        if(!page) {
                page = table->pages[page_num] = malloc(PAGE_SIZE);
        }
        uint32_t row_offset = row_num % ROWS_PER_PAGE;
        uint32_t byte_offset = row_offset * ROW_SIZE;
        return page + byte_offset;
}

Table* new_table() {
        Table* table = malloc(sizeof(Table));
        table->num_rows = 0;

        return table;
}

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
        ssize_t bytes_read = getline(&(input_buffer->buffer),
                                     &(input_buffer->buffer_length), stdin);

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
                int args_assigned = sscanf(
                        input_buffer->buffer, "Insert %d %s %s",
                        &(statment->row_to_insert.id),
                        statment->row_to_insert.username, statment->row_to_insert.email);
                if (args_assigned < 3) {
                        return PREPARE_SUCCESS;
                }
        }
        if (strcmp(input_buffer->buffer, "select") == 0) {
                statment->type = STATMENT_SELECT;
                return PREPARE_SUCCESS;
        }

        return PREPARE_UNRECOGNIZED_STATMENT;
}

ExecuteResult execute_insert(Statment* statement, Table* table) {
        if (table->num_rows >= TABLE_MAX_ROWS) {
                return EXECUTE_TABLE_FULL;
        }

        Row* row_to_insert = &(statement->row_to_insert);
        serialize_row(row_to_insert, row_slot(table, table->num_rows));
        table->num_rows += 1;
        return EXECUTE_SUCCESS;
}
ExecuteResult execute_select(Statment* statement, Table* table) {
        Row row;
        for (uint32_t i = 0; i< table->num_rows; i++) {
                deserialize_row(row_slot(table, i), &row);
                print_row(&row);
        }
        return EXECUTE_SUCCESS;
}
ExecuteResult execute_statment(Statment* statement, Table* table) {
        switch (statement->type) {
        case(STATMENT_INSERT):
                return execute_select(statement, table);
        case (STATMENT_SELECT):
                return execute_select(statement, table);
        }
}

int main(int argc, char* argv[])
{
        Table* table = new_table();
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
                case (PREPARE_SYNTAX_ERROR):
                        printf("Syntax error. Could not parse statment.\n");
                        continue;
                case (PREPARE_UNRECOGNIZED_STATMENT):
                        printf("Unrecognized keyword at start of '%s'\n",
                               input_buffer->buffer);
                        continue;
                }

                switch (execute_statment(&statment, table)) {
                case (EXIT_SUCCESS):
                        printf("Executed.\n");
                        break;
                case (EXECUTE_TABLE_FULL):
                        printf("Error: Table full.\n");
                        break;
                }
        }
}
