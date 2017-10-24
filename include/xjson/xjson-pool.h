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

#ifndef XJSON_POOL_H
#define XJSON_POOL_H

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <xjson/xjson.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// C O N F I G
// ---------------------------------------------------------------------------------------------------------------------

#ifndef XJSON_POOL_CAPACITY
    #define XJSON_POOL_CAPACITY                        500
#endif

// ---------------------------------------------------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct xjson_object_t         xjson_object_t;

typedef struct xjson_array_t          xjson_array_t;

typedef struct xjson_unnamed_entry_t  xjson_unnamed_entry_t;

typedef struct xjson_named_entry_t    xjson_named_entry_t;

typedef struct xjson_element_t        xjson_element_t;

typedef struct xjson_value_t          xjson_value_t;

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E   D E C L A R A T I O N S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct xjson_pool_t           xjson_pool_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

xjson_status_e xjson_pool_create(xjson_pool_t **pool);

xjson_status_e xjson_pool_dispose(xjson_pool_t *pool);

void *xjson_pool_malloc(xjson_pool_t *pool, xjson_u64_t size);


#ifdef __cplusplus
}
#endif

#endif //XJSON_POOL_H
