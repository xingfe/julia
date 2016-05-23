#ifndef _JULIA_BUFFER_H_
#define _JULIA_BUFFER_H_

#include "server.h"
#include "string.h"
#include "util.h"

#include <memory.h>
#include <stdbool.h>

typedef struct {
    char* begin;
    char* end;
    char* limit;
    char data[RECV_BUF_SIZE + 1];
} buffer_t;

static inline void buffer_init(buffer_t* buffer)
{
    buffer->begin = buffer->data;
    buffer->end = buffer->data;
    buffer->limit = buffer->data + RECV_BUF_SIZE;
}

static inline void buffer_clear(buffer_t* buffer)
{
    buffer_init(buffer);
}

static inline int buffer_size(buffer_t* buffer)
{
    return buffer->end - buffer->begin;
}

static inline int buffer_append(buffer_t* buffer, const char* data, int len)
{
    int appended_len = min(buffer->limit - buffer->end, len);
    memcpy(buffer->end, data, appended_len);
    buffer->end += appended_len;
    return appended_len;
}

// Discard the received data that has been successfully parsed.
// The remaining data is copied to the head of the buffer for next parsing.
static inline void buffer_discard_parsed(buffer_t* buffer)
{
    int size = buffer_size(buffer);
    // TODO(wgtdkp): Is it safe?
    // There shouldn't be much data remained
    memcpy(buffer->data, buffer->begin, size);
    buffer->begin = buffer->data;
    buffer->end = buffer->begin + size;
}

int buffer_recv(buffer_t* buffer, int fd);
int buffer_send(buffer_t* buffer, int fd);
int buffer_append_string(buffer_t* buffer, const string_t str);
int buffer_print(buffer_t* buffer, const char* format, ...);

#define buffer_append_cstring(buffer, cstr) buffer_append_string(buffer, STRING(cstr))

#endif