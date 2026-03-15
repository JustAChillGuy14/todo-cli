#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define IS_LITTLE_ENDIAN 1
#else
#define IS_LITTLE_ENDIAN 0
#endif

// helper functions to ensure endianness isn't broken

#if IS_LITTLE_ENDIAN

static inline size_t le_fwrite_1(uint8_t val, FILE *stream) { return fwrite(&val, 1, sizeof(val), stream); }
static inline size_t le_fwrite_2(uint16_t val, FILE *stream) { return fwrite(&val, 1, sizeof(val), stream); }
static inline size_t le_fwrite_4(uint32_t val, FILE *stream) { return fwrite(&val, 1, sizeof(val), stream); }
static inline size_t le_fwrite_8(uint64_t val, FILE *stream) { return fwrite(&val, 1, sizeof(val), stream); }

static inline size_t le_fread_1(uint8_t *val, FILE *stream) { return fread(val, 1, sizeof(*val), stream); }
static inline size_t le_fread_2(uint16_t *val, FILE *stream) { return fread(val, 1, sizeof(*val), stream); }
static inline size_t le_fread_4(uint32_t *val, FILE *stream) { return fread(val, 1, sizeof(*val), stream); }
static inline size_t le_fread_8(uint64_t *val, FILE *stream) { return fread(val, 1, sizeof(*val), stream); }

#else

size_t le_fwrite_1(uint8_t val, FILE *stream)
{
    return fwrite(&val, 1, sizeof(val), stream);
}

size_t le_fwrite_2(uint16_t val, FILE *stream)
{
    uint8_t bytes[sizeof(val)] = {val & 0xFF, val >> 8};
    return fwrite(bytes, 1, sizeof(bytes), stream);
}

size_t le_fwrite_4(uint32_t val, FILE *stream)
{
    uint8_t bytes[sizeof(val)] = {val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, (val >> 24) & 0xFF};
    return fwrite(bytes, 1, sizeof(bytes), stream);
}

size_t le_fwrite_8(uint64_t val, FILE *stream)
{

    uint8_t bytes[sizeof(val)] = {val & 0xFF, (val >> 8) & 0xFF, (val >> 16) & 0xFF, (val >> 24) & 0xFF, (val >> 32) & 0xFF, (val >> 40) & 0xFF, (val >> 48) & 0xFF, (val >> 56) & 0xFF};
    return fwrite(bytes, 1, sizeof(bytes), stream);
}

size_t le_fread_1(uint8_t *val, FILE *stream)
{
    return fread(val, 1, 1, stream);
}

size_t le_fread_2(uint16_t *val, FILE *stream)
{
    uint8_t bytes[2];
    if (fread(bytes, 1, 2, stream) != 2)
        return 0;
    *val = (uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8);
    return 2;
}

size_t le_fread_4(uint32_t *val, FILE *stream)
{
    uint8_t bytes[4];
    if (fread(bytes, 1, 4, stream) != 4)
        return 0;
    *val = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
    return 4;
}

size_t le_fread_8(uint64_t *val, FILE *stream)
{
    uint8_t bytes[8];
    if (fread(bytes, 1, 8, stream) != 8)
        return 0;
    *val = (uint64_t)bytes[0] | ((uint64_t)bytes[1] << 8) |
           ((uint64_t)bytes[2] << 16) | ((uint64_t)bytes[3] << 24) |
           ((uint64_t)bytes[4] << 32) | ((uint64_t)bytes[5] << 40) |
           ((uint64_t)bytes[6] << 48) | ((uint64_t)bytes[7] << 56);
    return 8;
}

#endif

// these macros clean up of a lot of random things

#define FATAL(failure)                                                 \
    do                                                                 \
    {                                                                  \
        printf(__FILE__ ":%s:%d: FATAL ERROR:\n", __func__, __LINE__); \
        perror((failure));                                             \
        exit(EXIT_FAILURE);                                            \
    } while (0)

/*
Stands for error malloc(allocate memory and fatally error if can't alloc)
*/
#define E_MALLOC(varname, s) \
    do                       \
    {                        \
        varname = malloc(s); \
        if (s && !varname)   \
        {                    \
            FATAL("malloc"); \
        }                    \
    } while (0)

#define E_FOPEN(file_var, filename, mode) \
    do                                    \
    {                                     \
        file_var = fopen(filename, mode); \
        if (!file_var)                    \
            FATAL("fopen");               \
    } while (0)

#define E_FSEEK(file, off, whence)        \
    do                                    \
    {                                     \
        if (fseek(file, off, whence) < 0) \
            FATAL("fseek");               \
    } while (0)

#define E_FOPEN_TASKS(file_var, filename, mode)         \
    do                                                  \
    {                                                   \
        file_var = fopen(filename, mode);               \
        if (!file_var)                                  \
        {                                               \
            if (errno == ENOENT)                        \
            {                                           \
                fprintf(stderr, "No tasks present.\n"); \
                exit(EXIT_FAILURE);                     \
            }                                           \
            FATAL("fopen");                             \
        }                                               \
    } while (0)

#define E_FWRITE_1(val, file)                      \
    do                                             \
    {                                              \
        if (le_fwrite_1(val, file) != sizeof(val)) \
        {                                          \
            FATAL("fwrite");                       \
        }                                          \
    } while (0)

#define E_FWRITE_2(val, file)                      \
    do                                             \
    {                                              \
        if (le_fwrite_2(val, file) != sizeof(val)) \
        {                                          \
            FATAL("fwrite");                       \
        }                                          \
    } while (0)

#define E_FWRITE_4(val, file)                      \
    do                                             \
    {                                              \
        if (le_fwrite_4(val, file) != sizeof(val)) \
        {                                          \
            FATAL("fwrite");                       \
        }                                          \
    } while (0)

#define E_FWRITE_8(val, file)                      \
    do                                             \
    {                                              \
        if (le_fwrite_8(val, file) != sizeof(val)) \
        {                                          \
            FATAL("fwrite");                       \
        }                                          \
    } while (0)

#define E_FREAD_1(var, file)                       \
    do                                             \
    {                                              \
        if (le_fread_1(&var, file) != sizeof(var)) \
        {                                          \
            FATAL("fread");                        \
        }                                          \
    } while (0)

#define E_FREAD_2(var, file)                       \
    do                                             \
    {                                              \
        if (le_fread_2(&var, file) != sizeof(var)) \
        {                                          \
            FATAL("fread");                        \
        }                                          \
    } while (0)

#define E_FREAD_4(var, file)                       \
    do                                             \
    {                                              \
        if (le_fread_4(&var, file) != sizeof(var)) \
        {                                          \
            FATAL("fread");                        \
        }                                          \
    } while (0)

#define E_FREAD_8(var, file)                       \
    do                                             \
    {                                              \
        if (le_fread_8(&var, file) != sizeof(var)) \
        {                                          \
            FATAL("fread");                        \
        }                                          \
    } while (0)

#define TODO_FILE ".todo"
#define TODO_TID_COUNT TODO_FILE ".tid" // incase we ever change TODO_FILE

#define SUBCOMMAND_NONE 0x0
#define SUBCOMMAND_DONE 0x1
#define SUBCOMMAND_LIST 0x2
#define SUBCOMMAND_ADD 0x3
#define SUBCOMMAND_RM 0x4

#define PRIORITY_DONE 0x0
#define PRIORITY_REMOVED 0x1
#define PRIORITY_DEFAULT 0x2
#define PRIORITY_LOW 0x55
#define PRIORITY_MID 0xaa
#define PRIORITY_HIGH 0xff

#define SUBLIST_DEFAULT 0x0

static void indent(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        printf("    ");
    }
}

// indent and print without ln not used anywhere and not needed.

static void indent_and_println(size_t depth, const char *fmt, ...)
{
    indent(depth);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    putchar('\n');
}

void usage_add(size_t depth)
{
    indent_and_println(depth, "add <message>: add a task to the list with specified message");
    indent_and_println(depth, "options:");
    indent_and_println(depth, "    --priority/-p <low/medium/high/(2..255)>: set priority of task to be specified priority");
    indent_and_println(depth, "    --help/-h: print usage of add subcommand");
}

void usage_rm(size_t depth)
{
    indent_and_println(depth, "rm <task-id>: remove a specific task");
    indent_and_println(depth, "options:");
    indent_and_println(depth, "    --help/-h: print usage of rm subcommand");
}

void usage_list(size_t depth)
{
    indent_and_println(depth, "list: list all tasks");
    indent_and_println(depth, "options:");
    indent_and_println(depth, "    --priority/-p <min-priority>: show only tasks with priority higher than minimum priority");
    indent_and_println(depth, "    --help/-h: print usage of list subcommand");
}

void usage_done(size_t depth)
{
    indent_and_println(depth, "done <task-id>: mark specific task done");
    indent_and_println(depth, "options:");
    indent_and_println(depth, "    --help/-h: print usage of done subcommand");
}

void usage(const char *our_name)
{
    printf("%s [subcommand] [option]... [arg]\n", our_name);
    printf("options:\n");
    printf("    --help/-h: print usage\n");
    printf("    --file/-f <file>: do subcommand only on one file\n");
    printf("    --sublist/-s <sublist id>: do subcommand on specified sublist id\n");
    printf("subcommands:\n");
    usage_add(1);
    printf("\n");
    usage_list(1);
    printf("\n");
    usage_done(1);
    printf("\n");
    usage_rm(1);
}

void usage_subcommand(uint8_t subcommand, const char *our_name)
{
    if (subcommand == SUBCOMMAND_NONE)
    {
        usage(our_name);
    }
    else if (subcommand == SUBCOMMAND_ADD)
    {
        usage_add(0);
    }
    else if (subcommand == SUBCOMMAND_RM)
    {
        usage_rm(0);
    }
    else if (subcommand == SUBCOMMAND_LIST)
    {
        usage_list(0);
    }
    else if (subcommand == SUBCOMMAND_DONE)
    {
        usage_done(0);
    }
}

void add(const char *file, const char *msg, uint8_t priority, uint32_t sublist)
{
    uint32_t file_len = (uint32_t)strlen(file);
    uint32_t msg_len = (uint32_t)strlen(msg);

    uint32_t tid = 0;
    bool found = false;

    FILE *tid_count;
    E_FOPEN(tid_count, TODO_TID_COUNT, "r+b");

    uint32_t tid_flen;
    char *tid_f;

    long tid_start;

    uint32_t skip_tid;

    while (1)
    {
        if (le_fread_4(&tid_flen, tid_count) != sizeof(tid_flen))
            break; // nothing there

        if (tid_flen != file_len)
        {
            E_FSEEK(tid_count, tid_flen, SEEK_CUR);
            E_FREAD_4(skip_tid, tid_count); // since we wanna skip over this tid
            continue;
        }

        E_MALLOC(tid_f, tid_flen);

        fread(tid_f, tid_flen, 1, tid_count);
        if (memcmp(tid_f, file, file_len)) // if they AREN'T equal
        {
            free(tid_f);
            E_FREAD_4(tid_flen, tid_count);
            continue;
        }
        tid_start = ftell(tid_count);
        if (tid_start == -1L)
        {
            FATAL("ftell");
        }
        E_FREAD_4(tid, tid_count);
        tid++; // get NEXT tid

        E_FSEEK(tid_count, tid_start, SEEK_SET);

        E_FWRITE_4(tid, tid_count);
        found = true;
        free(tid_f);
        break;
    }

    if (!found)
    {
        // not found, so tid = 0

        E_FSEEK(tid_count, 0, SEEK_END);
        E_FWRITE_4(file_len, tid_count);
        fwrite(file, file_len, 1, tid_count);
        tid++; // because we will actually operate on `1` NOT `0` this time.
        E_FWRITE_4(tid, tid_count);

        FILE *todo_file;
        E_FOPEN(todo_file, TODO_FILE, "ab");

        E_FWRITE_4(file_len, todo_file);
        fwrite(file, file_len, 1, todo_file);
        tid--; // restoration
        E_FWRITE_4(tid, todo_file);

        uint32_t zero = 0;
        uint8_t root_prio = PRIORITY_DEFAULT;

        // priority, parent, child-count, child-done-count, message-len

        E_FWRITE_1(root_prio, todo_file);
        E_FWRITE_4(zero, todo_file); // yes, 0's parent is 0, because we don't have to validate anything :)
        E_FWRITE_4(zero, todo_file); // 0 children (currently)
        E_FWRITE_4(zero, todo_file); // 0 children who are marked done (currently)
        E_FWRITE_4(zero, todo_file); // 0 is the length of the root's message

        tid++; // get tid to be 1 instead of 0

        fclose(todo_file);
    }

    fclose(tid_count);

    if (sublist >= tid)
    {
        fprintf(stderr, "sublist id %" PRIu32 " doesn't exist.\n", sublist);
        exit(EXIT_FAILURE);
    }

    uint32_t flen;
    char *f;
    uint32_t id;
    uint8_t prio;
    uint32_t parent_id;
    uint32_t child_count;

    // NOTE: we don't need this:
    // uint32_t child_done_count;
    // uint32_t mlen;
    // char *m;

    // ensure TODO_FILE exists

    FILE *todo_file;

    // we know it exists because we make it at tid = 0

    E_FOPEN(todo_file, TODO_FILE, "r+b");

    while (true)
    {
        E_FREAD_4(flen, todo_file);

        if (flen != file_len)
        {
            E_FSEEK(todo_file, flen, SEEK_CUR);
            E_FREAD_4(id, todo_file);
            E_FREAD_1(prio, todo_file);
            E_FREAD_4(parent_id, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FSEEK(todo_file, sizeof(uint32_t), SEEK_CUR); // skip child_done_count
            E_FREAD_4(flen, todo_file);                     // get len(into flen because we don't have mlen)
            E_FSEEK(todo_file, flen, SEEK_CUR);
            continue;
        }

        E_MALLOC(f, flen);

        fread(f, flen, 1, todo_file);

        if (memcmp(f, file, flen))
        {
            // NOT equal
            free(f);
            E_FREAD_4(id, todo_file);
            E_FREAD_1(prio, todo_file);
            E_FREAD_4(parent_id, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FSEEK(todo_file, sizeof(uint32_t), SEEK_CUR); // skip child_done_count
            E_FREAD_4(flen, todo_file);                     // get len(into flen because we don't have mlen)
            E_FSEEK(todo_file, flen, SEEK_CUR);
            continue;
        }

        E_FREAD_4(id, todo_file);

        if (id != sublist)
        {
            free(f);
            E_FREAD_1(prio, todo_file);
            E_FREAD_4(parent_id, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FSEEK(todo_file, sizeof(uint32_t), SEEK_CUR); // skip child_done_count
            E_FREAD_4(flen, todo_file);                     // get len(into flen because we don't have mlen)
            E_FSEEK(todo_file, flen, SEEK_CUR);
            continue;
        }

        // our match

        E_FREAD_1(prio, todo_file);

        if (prio == PRIORITY_REMOVED)
        {
            fprintf(stderr, "Sublist has already been removed.\n");
            exit(EXIT_FAILURE);
        }

        if (prio == PRIORITY_DONE)
        {
            fprintf(stderr, "Sublist has already been marked done.\n");
            exit(EXIT_FAILURE);
        }

        E_FREAD_4(parent_id, todo_file); // nothing to do with it though

        E_FREAD_4(child_count, todo_file);

        E_FSEEK(todo_file, -4, SEEK_CUR); // writing mode INITIATED!!(go backwards to write the NEW child_count)

        child_count++;

        E_FWRITE_4(child_count, todo_file);

        free(f);

        break;
    }

    E_FSEEK(todo_file, 0, SEEK_END);

    child_count = 0;               // default at the start (we already have it from the sublist search)
    uint32_t child_done_count = 0; // default at the start

    E_FWRITE_4(file_len, todo_file);
    fwrite(file, file_len, 1, todo_file);
    E_FWRITE_4(tid, todo_file);
    E_FWRITE_1(priority, todo_file);
    E_FWRITE_4(sublist, todo_file);
    E_FWRITE_4(child_count, todo_file);
    E_FWRITE_4(child_done_count, todo_file);
    E_FWRITE_4(msg_len, todo_file);
    fwrite(msg, msg_len, 1, todo_file);

    fclose(todo_file);
}

void list(const char *file, uint8_t priority, uint32_t sublist)
{
    if (*file)
        printf("todo list of file %s:\n", file);
    else
        printf("full todo list:\n");
    uint32_t file_len = (uint32_t)strlen(file);

    FILE *todo_file;

    E_FOPEN_TASKS(todo_file, TODO_FILE, "rb");

    char *gotten_file;
    uint32_t gotten_file_len;
    uint32_t tid;
    uint8_t gotten_priority;
    uint32_t parent;
    uint32_t child_count;
    uint32_t child_done_count;
    uint32_t msg_len;
    char *msg;

    while (true)
    {
        if (le_fread_4(&gotten_file_len, todo_file) != sizeof(gotten_file_len)) // nothing left
            break;

        if (gotten_file_len != file_len)
        {
            // not our file!
            E_FSEEK(todo_file, gotten_file_len, SEEK_CUR);
            E_FREAD_4(tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(parent, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FREAD_4(child_done_count, todo_file);
            E_FREAD_4(gotten_file_len, todo_file); // since we re-read next iteration, we don't care about the value of gotten_file_len
            E_FSEEK(todo_file, gotten_file_len, SEEK_CUR);
            continue;
        }

        E_MALLOC(gotten_file, gotten_file_len);

        fread(gotten_file, gotten_file_len, 1, todo_file);

        if (memcmp(gotten_file, file, file_len)) // if NOT equal
        {
            free(gotten_file);
            E_FREAD_4(tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(parent, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FREAD_4(child_done_count, todo_file);
            E_FREAD_4(msg_len, todo_file);
            E_FSEEK(todo_file, msg_len, SEEK_CUR);
            continue;
        }

        E_FREAD_4(tid, todo_file);

        // they are equal
        E_FREAD_1(gotten_priority, todo_file);
        if (gotten_priority < priority && !(priority <= PRIORITY_DEFAULT && gotten_priority == PRIORITY_DONE))
        {
            free(gotten_file);
            E_FREAD_4(parent, todo_file);
            E_FREAD_4(child_count, todo_file);
            E_FREAD_4(child_done_count, todo_file);
            E_FREAD_4(msg_len, todo_file);
            E_FSEEK(todo_file, msg_len, SEEK_CUR);
            continue; // skip it
        }

        E_FREAD_4(parent, todo_file);

        if (parent != sublist)
        {
            free(gotten_file);
            E_FREAD_4(child_count, todo_file);
            E_FREAD_4(child_done_count, todo_file);
            E_FREAD_4(msg_len, todo_file);
            E_FSEEK(todo_file, msg_len, SEEK_CUR);
            continue; // another skip
        }

        if (parent == 0 && sublist == 0 && tid == 0) // root task
        {
            free(gotten_file);
            E_FREAD_4(child_count, todo_file);
            E_FREAD_4(child_done_count, todo_file);
            E_FREAD_4(msg_len, todo_file);
            E_FSEEK(todo_file, msg_len, SEEK_CUR);
            continue; // skip root task
        }

        E_FREAD_4(child_count, todo_file);
        E_FREAD_4(child_done_count, todo_file);
        E_FREAD_4(msg_len, todo_file);

        E_MALLOC(msg, msg_len + 1);

        fread(msg, msg_len, 1, todo_file);

        msg[msg_len] = '\0'; // null termination

        printf("[%" PRIu32 "] ", tid);

        printf(gotten_priority == PRIORITY_DONE ? "[x] " : "[ ] "); // yellow/green(with [x]/[ ])

        if (child_count > 0)
            printf("<sublist> ");

        printf(gotten_priority == PRIORITY_DONE ? "\033[32m" : "\033[33m");

        printf("%s", msg);

        printf("\033[0m\n"); // restoration

        free(gotten_file);
        free(msg);
    }

    fclose(todo_file);
}

uint32_t parse_tid(const char *arg); // done and rm use it

void done(const char *file, const char *arg, uint32_t sublist)
{
    uint32_t tid = parse_tid(arg);
    uint32_t file_len = strlen(file);

    FILE *todo_file;

    E_FOPEN_TASKS(todo_file, TODO_FILE, "r+b");

    uint32_t gotten_file_len;
    char *gotten_file;
    uint8_t gotten_priority;
    uint32_t gotten_tid;
    uint32_t gotten_msg_len;
    uint32_t gotten_parent;
    uint32_t gotten_child_count;
    uint32_t gotten_child_done_count;

    long parent_start = 0;

    bool made_done = false;

    while (true)
    {
        if (le_fread_4(&gotten_file_len, todo_file) != sizeof(gotten_file_len))
            break; // nothing left

        if (gotten_file_len != file_len)
        {
            E_FSEEK(todo_file, gotten_file_len, SEEK_CUR);
            E_FREAD_4(gotten_tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_MALLOC(gotten_file, gotten_file_len);

        fread(gotten_file, gotten_file_len, 1, todo_file);

        if (memcmp(gotten_file, file, file_len)) // if NOT equal
        {
            free(gotten_file);
            E_FREAD_4(gotten_tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_FREAD_4(gotten_tid, todo_file);

        if (gotten_tid == sublist)
        {
            parent_start = ftell(todo_file);
            if (parent_start == -1L)
                FATAL("ftell");

            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        if (gotten_tid != tid)
        {
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_FREAD_1(gotten_priority, todo_file);

        if (gotten_priority == PRIORITY_DONE)
        {
            fprintf(stderr, "Task has already been marked done\n");
            exit(EXIT_FAILURE);
        }

        if (gotten_priority == PRIORITY_REMOVED)
        {
            fprintf(stderr, "Task has already been removed\n");
            exit(EXIT_FAILURE);
        }

        E_FREAD_4(gotten_parent, todo_file);

        if (gotten_parent != sublist)
        {
            fprintf(stderr, "Incorrect sublist specified to mark this task done.\n");
            fprintf(stderr, "The correct sublist is: %" PRIu32 "\n", gotten_parent);
            exit(EXIT_FAILURE);
        }

        E_FREAD_4(gotten_child_count, todo_file);
        E_FREAD_4(gotten_child_done_count, todo_file);

        if (gotten_child_count != gotten_child_done_count)
        {
            fprintf(stderr, "Cannot mark this sublist done as it still has children\n");
            exit(EXIT_FAILURE);
        }

        E_FSEEK(todo_file, -sizeof(gotten_child_done_count) - sizeof(gotten_child_count) - sizeof(gotten_parent) - sizeof(gotten_priority), SEEK_CUR); // just go back to priority
        gotten_priority = PRIORITY_DONE;                                                                                                               // set it to done
        E_FWRITE_1(gotten_priority, todo_file);
        made_done = true;
        break;
    }

    if (!made_done)
    {
        if (*file)
            fprintf(stderr, "No such task id %" PRIu32 " in file `%s`\n", tid, file);
        else
            fprintf(stderr, "No such task id %" PRIu32 "\n", tid);
        exit(EXIT_FAILURE);
    }

    // successful case, so we update the parent
    E_FSEEK(todo_file, parent_start, SEEK_SET);

    E_FREAD_1(gotten_priority, todo_file);
    if (gotten_priority == PRIORITY_DONE)
    {
        fprintf(stderr, "Parent has already been marked done\n");
    }

    if (gotten_priority == PRIORITY_REMOVED)
    {
        fprintf(stderr, "Parent has already been removed\n");
    }

    E_FREAD_4(gotten_parent, todo_file); // we don't care about the parent of the parent

    E_FREAD_4(gotten_child_count, todo_file); // we don't care about the child_count of the parent
    
    E_FREAD_4(gotten_child_done_count, todo_file);
    E_FSEEK(todo_file, -sizeof(gotten_child_done_count), SEEK_CUR);
    gotten_child_done_count++;
    E_FWRITE_4(gotten_child_done_count, todo_file);

    fclose(todo_file);
}

void rm(const char *file, const char *arg, uint32_t sublist)
{
    uint32_t tid = parse_tid(arg);
    uint32_t file_len = strlen(file);

    FILE *todo_file;

    E_FOPEN_TASKS(todo_file, TODO_FILE, "r+b");

    uint32_t gotten_file_len;
    char *gotten_file;
    uint8_t gotten_priority;
    uint32_t gotten_tid;
    uint32_t gotten_msg_len;
    uint32_t gotten_parent;
    uint32_t gotten_child_count;
    uint32_t gotten_child_done_count;

    long parent_start = 0;

    bool removed = false;

    while (true)
    {
        if (le_fread_4(&gotten_file_len, todo_file) != sizeof(gotten_file_len))
            break; // nothing left

        if (gotten_file_len != file_len)
        {
            E_FSEEK(todo_file, gotten_file_len, SEEK_CUR);
            E_FREAD_4(gotten_tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_MALLOC(gotten_file, gotten_file_len);

        fread(gotten_file, gotten_file_len, 1, todo_file);

        if (memcmp(gotten_file, file, file_len)) // if NOT equal
        {
            free(gotten_file);
            E_FREAD_4(gotten_tid, todo_file);
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_FREAD_4(gotten_tid, todo_file);

        if (gotten_tid == sublist)
        {
            parent_start = ftell(todo_file);
            if (parent_start == -1L)
                FATAL("ftell");

            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        if (gotten_tid != tid)
        {
            E_FREAD_1(gotten_priority, todo_file);
            E_FREAD_4(gotten_parent, todo_file);
            E_FREAD_4(gotten_child_count, todo_file);
            E_FREAD_4(gotten_child_done_count, todo_file);
            E_FREAD_4(gotten_msg_len, todo_file);
            E_FSEEK(todo_file, gotten_msg_len, SEEK_CUR);
            continue;
        }

        E_FREAD_1(gotten_priority, todo_file);

        if (gotten_priority == PRIORITY_REMOVED)
        {
            fprintf(stderr, "Task has already been removed\n");
            exit(EXIT_FAILURE);
        }

        E_FREAD_4(gotten_parent, todo_file);

        if (gotten_parent != sublist)
        {
            fprintf(stderr, "Incorrect sublist specified to mark this task done.\n");
            fprintf(stderr, "The correct sublist is: %" PRIu32 "\n", gotten_parent);
            exit(EXIT_FAILURE);
        }

        E_FREAD_4(gotten_child_count, todo_file);
        E_FREAD_4(gotten_child_done_count, todo_file);

        if (gotten_child_count > 0)
        {
            fprintf(stderr, "Cannot mark this sublist done as it still has children\n");
            exit(EXIT_FAILURE);
        }

        E_FSEEK(todo_file, -sizeof(gotten_child_done_count) - sizeof(gotten_child_count) - sizeof(gotten_parent) - sizeof(gotten_priority), SEEK_CUR); // just go back to priority
        gotten_priority = PRIORITY_DONE;                                                                                                               // set it to done
        E_FWRITE_1(gotten_priority, todo_file);
        removed = true;
        break;
    }

    if (!removed)
    {
        if (*file)
            fprintf(stderr, "No such task id %" PRIu32 " in file `%s`\n", tid, file);
        else
            fprintf(stderr, "No such task id %" PRIu32 "\n", tid);
        exit(EXIT_FAILURE);
    }

    // successful case, so we update the parent
    E_FSEEK(todo_file, parent_start, SEEK_SET);

    E_FREAD_1(gotten_priority, todo_file);
    if (gotten_priority == PRIORITY_REMOVED)
    {
        fprintf(stderr, "Parent has already been removed\n");
    }

    E_FREAD_4(gotten_parent, todo_file); // we don't care about the parent of the parent

    E_FREAD_4(gotten_child_count, todo_file);
    E_FSEEK(todo_file, -sizeof(gotten_child_count), SEEK_CUR); // go back to child_count as it represents non-removed, meaning decrementation!!
    gotten_child_count--;
    E_FWRITE_4(gotten_child_count, todo_file);

    fclose(todo_file);
}

uint32_t parse_tid(const char *arg) // needed by `done` and `rm`
{
    uint32_t value = 0;
    for (const char *s = arg; *s; s++)
    {
        if (*s < '0' || *s > '9')
        {
            fprintf(stderr, "Unrecognized task id: %s\n", arg);
            exit(EXIT_FAILURE);
        }
        uint8_t digit = *s - '0';
        if (value > (UINT32_MAX - digit) / 10)
        {
            fprintf(stderr, "Task id too high: %s\n", arg);
            exit(EXIT_FAILURE);
        }
        value = value * 10 + digit;
    }
    return value;
}

static void ensure_tid_file(void)
{
    FILE *tid_count = fopen(TODO_TID_COUNT, "r");
    if (!tid_count)
    {
        if (errno != ENOENT)
            FATAL("fopen");

        E_FOPEN(tid_count, TODO_TID_COUNT, "w");
    }
    fclose(tid_count);
}

static uint8_t parse_priority(const char *str)
{
    if (!strcmp(str, "low"))
        return PRIORITY_LOW;
    if (!strcmp(str, "medium"))
        return PRIORITY_MID;
    if (!strcmp(str, "high"))
        return PRIORITY_HIGH;

    uint8_t value = 0;
    for (const char *s = str; *s; s++)
    {
        if (*s < '0' || *s > '9')
        {
            fprintf(stderr, "Unrecognized priority: %s\n", str);
            exit(EXIT_FAILURE);
        }
        uint8_t digit = *s - '0';
        if (value > (255 - digit) / 10)
        {
            fprintf(stderr, "Priority too high: %s\n", str);
            exit(EXIT_FAILURE);
        }
        value = value * 10 + digit;
    }
    if (value == PRIORITY_REMOVED || value == PRIORITY_DONE)
    {
        fprintf(stderr, "Priority cannot be %" PRIu8 "\n", value);
        exit(EXIT_FAILURE);
    }
    return value;
}

static uint32_t parse_sublist(const char *arg)
{
    uint32_t value = 0;
    for (const char *s = arg; *s; s++)
    {
        if (*s < '0' || *s > '9')
        {
            fprintf(stderr, "Unrecognized sublist id: %s\n", arg);
            exit(EXIT_FAILURE);
        }
        uint8_t digit = *s - '0';
        if (value > (UINT32_MAX - digit) / 10)
        {
            fprintf(stderr, "Sublist id too high: %s\n", arg);
            exit(EXIT_FAILURE);
        }
        value = value * 10 + digit;
    }
    return value;
}

typedef struct
{
    uint8_t subcommand;
    char *file;
    char *arg;
    uint8_t priority;
    uint32_t sublist; // essentially the parent
} cli_state;

#define REQUIRE_ARG(subcmd, i, argc)                           \
    do                                                         \
    {                                                          \
        if (i + 1 >= argc)                                     \
        {                                                      \
            fprintf(stderr, "%s requires argument\n", subcmd); \
            exit(EXIT_FAILURE);                                \
        }                                                      \
    } while (0)

#define HANDLE_ARG_HELP(state, argv)                  \
    do                                                \
    {                                                 \
        usage_subcommand(state->subcommand, argv[0]); \
        exit(EXIT_SUCCESS);                           \
    } while (0)

#define HANDLE_ARG_FILE(f, state, i, argc, argv) \
    do                                           \
    {                                            \
        REQUIRE_ARG(f, i, argc);                 \
        state->file = argv[++i];                 \
    } while (0)

#define HANDLE_ARG_PRIO(p, state, i, argc, argv)     \
    do                                               \
    {                                                \
        REQUIRE_ARG(p, i, argc);                     \
        state->priority = parse_priority(argv[++i]); \
    } while (0)

#define HANDLE_ARG_SUBL(s, state, i, argc, argv)   \
    do                                             \
    {                                              \
        REQUIRE_ARG(s, i, argc);                   \
        state->sublist = parse_sublist(argv[++i]); \
    } while (0)

#define UNRECOGNIZED_OPTION(arg)                            \
    do                                                      \
    {                                                       \
        fprintf(stderr, "Unrecognized option `%s`\n", arg); \
        exit(EXIT_FAILURE);                                 \
    } while (0)

static void parse_args(int argc, char **argv, cli_state *state)
{
    state->subcommand = SUBCOMMAND_NONE;
    state->file = "";
    state->arg = NULL;
    state->priority = PRIORITY_DEFAULT; // lowest priority, lower than PRIORITY_LOW
    state->sublist = SUBLIST_DEFAULT;   // 0

    bool flagend = false;

    for (int i = 1; i < argc; i++)
    {
        char *arg = argv[i];

        if (!flagend && arg[0] == '-')
        {
            // check for flag-end
            if (arg[1] == '-' && arg[2] == '\0')
            {
                flagend = true;
                continue;
            }

            // long flags
            if (arg[1] == '-')
            {
                if (!strcmp(arg, "--help"))
                    HANDLE_ARG_HELP(state, argv);
                else if (!strcmp(arg, "--file"))
                    HANDLE_ARG_FILE("--file", state, i, argc, argv);
                else if (!strcmp(arg, "--priority"))
                    HANDLE_ARG_PRIO("--priority", state, i, argc, argv);
                else if (!strcmp(arg, "--sublist"))
                    HANDLE_ARG_SUBL("--sublist", state, i, argc, argv);
                else
                    UNRECOGNIZED_OPTION(arg);

                continue;
            }
            // short flags
            switch (arg[1])
            {
            case 'h':
                HANDLE_ARG_HELP(state, argv);
                break; // not needeed, but eh
            case 'f':
                HANDLE_ARG_FILE("-f", state, i, argc, argv);
                break;
            case 'p':
                HANDLE_ARG_PRIO("-p", state, i, argc, argv);
                break;
            case 's':
                HANDLE_ARG_SUBL("-s", state, i, argc, argv);
                break;
            default:
                UNRECOGNIZED_OPTION(arg);
            }
            continue;
        }

        // all non-flag arguments, or after flag-end
        if (state->subcommand != SUBCOMMAND_NONE)
        {
            if (state->arg)
            {
                fprintf(stderr, "Only one argument is allowed for each subcommand\n"); // maybe change in the future
                exit(EXIT_FAILURE);
            }
            state->arg = arg;
        }
        else if (!strcmp(arg, "add"))
            state->subcommand = SUBCOMMAND_ADD;
        else if (!strcmp(arg, "list"))
            state->subcommand = SUBCOMMAND_LIST;
        else if (!strcmp(arg, "done"))
            state->subcommand = SUBCOMMAND_DONE;
        else if (!strcmp(arg, "rm"))
            state->subcommand = SUBCOMMAND_RM;
        else
        {
            fprintf(stderr, "Unrecognized subcommand: %s\n", arg);
            exit(EXIT_FAILURE);
        }
    }

    if (state->subcommand == SUBCOMMAND_NONE)
    {
        fprintf(stderr, "No subcommand specified\n");
        exit(1);
    }
}

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    ensure_tid_file();

    cli_state state;
    parse_args(argc, argv, &state);

    switch (state.subcommand)
    {
    case SUBCOMMAND_ADD:
        if (!state.arg)
        {
            fprintf(stderr, "`add` expects a message\n");
            return 1;
        }
        add(state.file, state.arg, state.priority, state.sublist);
        break;
    case SUBCOMMAND_LIST:
        list(state.file, state.priority, state.sublist);
        break;
    case SUBCOMMAND_DONE:
        if (!state.arg)
        {
            fprintf(stderr, "`done` expects a task id\n");
            return 1;
        }
        done(state.file, state.arg, state.sublist);
        break;
    case SUBCOMMAND_RM:
        if (!state.arg)
        {
            fprintf(stderr, "`rm` expects a task id\n");
            return 1;
        }
        rm(state.file, state.arg, state.sublist);
        break;
    }

    return 0;
}