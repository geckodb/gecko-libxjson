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

#ifndef XJSON_OPERATOR_H
#define XJSON_OPERATOR_H

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <brooks/brooks.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_object_t   brooks_object_t;
typedef struct brooks_array_t    brooks_array_t;
typedef struct brooks_cursor_t   brooks_cursor_t;

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E   D E F I N I T I O N
// ---------------------------------------------------------------------------------------------------------------------

typedef enum brooks_opp_tag_e {
    brooks_opp_tag_scan_objects_default,
    brooks_opp_tag_scan_arrays_default
} brooks_opp_tag_e;

typedef struct brooks_operator_t
{
    brooks_status_e        (*open)(struct brooks_operator_t *self);
    brooks_status_e        (*close)(struct brooks_operator_t *self);
    const brooks_cursor_t *(*next)(struct brooks_operator_t *self);
    brooks_opp_tag_e         tag;
    void                    *extra;
} brooks_operator_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_operator_open(brooks_operator_t *opp);

const brooks_cursor_t *brooks_operator_next(brooks_operator_t *opp);

brooks_status_e brooks_operator_close(brooks_operator_t *opp);

#ifdef __cplusplus
}
#endif

#endif //XJSON_OPERATOR_H
