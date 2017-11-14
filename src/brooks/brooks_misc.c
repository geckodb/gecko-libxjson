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
#include <string.h>

#include <brooks/brooks_misc.h>

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

void *brooks_misc_pooled_autoresize(brooks_pool_t *pool, void *base, size_t elem_size, size_t num_entries,
                                    size_t *capacity, size_t num_add)
{
    size_t new_num_entires = num_entries + num_add;
    if (new_num_entires > *capacity) {
        while (new_num_entires >= *capacity) {
            *capacity = (*capacity + 1) * 1.7f;
        }
    }
    void *result = brooks_pool_malloc(pool, *capacity * elem_size);
    memcpy(result, base, num_entries * elem_size);
    return result;
}

void *brooks_misc_autoresize(void *base, size_t elem_size, size_t num_entries, size_t *capacity, size_t num_add)
{
    size_t new_num_entires = num_entries + num_add;
    if (new_num_entires > *capacity) {
        while (new_num_entires > *capacity) {
            *capacity = (*capacity + 1) * 1.7f;
        }
        return realloc(base, *capacity * elem_size);
    } else return base;
}

char *brooks_misc_strdup(brooks_pool_t *pool, const char *str)
{
    char *cpy = brooks_pool_malloc(pool, strlen(str) + 1);
    strcpy(cpy, str);
    return cpy;
}

