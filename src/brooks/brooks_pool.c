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

#include <brooks/brooks.h>
#include <brooks/brooks_pool.h>
#include <brooks/brooks_misc.h>

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_pool_t
{
    void **base;
    size_t num_elements;
    size_t capacity;
} brooks_pool_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_pool_create(brooks_pool_t **pool)
{
    brooks_pool_t *retval;
    if ((retval = malloc(sizeof(brooks_pool_t))) != NULL) {
        retval->capacity = BROOKS_POOL_CAPACITY;
        retval->base = malloc(retval->capacity * sizeof(void *));
        retval->num_elements = 0;
        *pool = retval;
        return ((retval->base != NULL) ? brooks_status_ok : brooks_status_malloc_err);
    }
    return brooks_status_malloc_err;
}

brooks_status_e brooks_pool_dispose(brooks_pool_t *pool)
{
    void **it = pool->base;
    while (pool->num_elements--) {
        free(*it);
        it++;
    }
    free(pool->base);
    free(pool);
    return brooks_status_malloc_err;
}

void *brooks_pool_malloc(brooks_pool_t *pool, size_t size)
{
    void *retval = malloc(size);
    pool->base = brooks_misc_autoresize(pool->base, sizeof(void *), pool->num_elements, &pool->capacity, 1);
    pool->base[pool->num_elements++] = retval;
    return retval;
}