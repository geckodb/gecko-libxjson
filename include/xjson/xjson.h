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

#ifndef XJSON_H
#define XJSON_H

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

#define XJSON_ROOT                   NULL

#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef enum
{
    xjson_status_ok             = 1,
    xjson_status_true           = 1,
    xjson_status_false          = 0,
    xjson_status_failed         = 0,
    xjson_status_malloc_err,
    xjson_status_pmalloc_err,
    xjson_status_realloc_err,
    xjson_status_nullptr        = 2,
    xjson_status_notype,
    xjson_status_interalerr,
    xjson_status_wrongusage,
    xjson_status_nopool,
    xjson_status_eof            = 0
}                                     xjson_status_e;

typedef long long                     xjson_s64_t;

typedef unsigned long long            xjson_u64_t;

typedef double                        xjson_double_t;

typedef char *                        xjson_string_t;

typedef int                           xjson_boolean_t;

#ifdef __cplusplus
}
#endif

#endif //XJSON_H
