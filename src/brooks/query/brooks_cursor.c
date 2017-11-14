//
// Copyright (C) 2017 Marcus Pinnecke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of
// the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <brooks/query/brooks_cursor.h>
#include <brooks/brooks_pool.h>
#include <string.h>
#include <brooks/brooks_misc.h>

// ---------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_cursor_t
{
    brooks_value_t        **values;
    size_t                  capacity;
    size_t                  num_values;
    brooks_pool_t           *pool;
} brooks_cursor_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------


brooks_status_e brooks_cursor_create(brooks_cursor_t **cursor, size_t capacity, brooks_pool_t *pool)
{
    if (cursor && capacity > 0) {
        brooks_cursor_t *result = malloc(sizeof(brooks_cursor_t));
        result->capacity = capacity;
        result->num_values = 0;
        result->values = brooks_pool_malloc(pool, capacity * sizeof(brooks_value_t *));
        result->pool = pool;
        *cursor = result;
        return brooks_status_ok;
    } else return brooks_status_illegalarg;
}

void brooks_cursor_append(brooks_cursor_t *cursor, const brooks_value_t * const *values, size_t num_values)
{
    size_t head = cursor->num_values;
    cursor->values = brooks_misc_pooled_autoresize(cursor->pool, cursor->values, sizeof(brooks_value_t  *),
                                            cursor->num_values, &cursor->capacity, num_values);
    memcpy(cursor->values + head, values, num_values * sizeof(brooks_value_t *));
    cursor->num_values += num_values;
}

void brooks_cursor_clear(brooks_cursor_t *cursor)
{
    if (cursor) {
        cursor->num_values = 0;
    }
}

brooks_value_t **brooks_cursor_read(size_t *num_elements, const brooks_cursor_t *cursor)
{
    if (num_elements && cursor) {
        *num_elements = cursor->num_values;
        return (cursor->values);
    } else return NULL;
}

brooks_status_e brooks_cursor_dispose(brooks_cursor_t *cursor)
{
    if (cursor) {
        free (cursor);
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}