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

#ifndef BROOKS_H
#define BROOKS_H

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// C O N S T A N T S
// ---------------------------------------------------------------------------------------------------------------------

#define BROOKS_OBJECT_ROOT                   NULL

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------------------------------------
// M A C R O S
// ---------------------------------------------------------------------------------------------------------------------

#define INIT_VECTOR(vector, sizeof_element, vec_capacity, pool)                             \
    {                                                                                       \
        vector.element_size = sizeof_element;                                               \
        vector.capacity = vec_capacity;                                                     \
        vector.base = brooks_pool_malloc(pool, vec_capacity * sizeof_element);              \
        vector.num_elements = 0;                                                            \
    }

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef enum
{
    brooks_status_ok             = 1,
    brooks_status_true           = 1,
    brooks_status_false          = 0,
    brooks_status_failed         = 0,
    brooks_status_malloc_err,
    brooks_status_pmalloc_err,
    brooks_status_realloc_err,
    brooks_status_nullptr        = 2,
    brooks_status_notype,
    brooks_status_interalerr,
    brooks_status_wrongusage,
    brooks_status_nopool,
    brooks_status_eof            = 0,
    brooks_status_illegalarg,
    brooks_status_badcall,
    brooks_status_full
}                                     brooks_status_e;

typedef enum
{
    brooks_entry_type_named_entry, brooks_entry_type_unnamed_entry
}                                     brooks_entry_type_e;

typedef enum
{
    brooks_type_none,

    brooks_type_object,
    brooks_type_array,

    brooks_type_number_integer,
    brooks_type_number_double,
    brooks_type_string,
    brooks_type_boolean,

    brooks_type_null
}                                     brooks_type_e;

typedef struct brooks_vector_t
{
    void *base;
    size_t num_elements;
    size_t capacity;
    size_t element_size;
}                                     brooks_vector_t;

#ifdef __cplusplus
}
#endif

#endif //BROOKS
