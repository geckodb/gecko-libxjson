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
#include <brooks/query/operators/scans/brooks_scan_objects.h>
#include <brooks/query/brooks_cursor.h>
#include <brooks/brooks_doc.h>

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct scan_objects_extra_t
{
    brooks_object_t * const *objects;
    size_t                   num_objects;
    size_t                   current_object_idx;
    brooks_cursor_t         *cursor;
    brooks_pool_t           *pool;
} scan_objects_extra_t;

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e scan_objects_open(struct brooks_operator_t *self);
brooks_status_e scan_objects_close(struct brooks_operator_t *self);
const brooks_cursor_t *scan_objects_next(struct brooks_operator_t *self);

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_operators_scan_objects_create(brooks_operator_t *opp, brooks_object_t * const * objs, size_t num,
                                                     brooks_pool_t *pool)
{
    if (opp && objs && num > 0) {
        scan_objects_extra_t *extra = malloc(sizeof(scan_objects_extra_t));
        extra->current_object_idx = 0;
        extra->num_objects = num;
        extra->objects = objs;
        extra->pool = pool;

        opp->extra = extra;
        opp->tag = brooks_opp_tag_scan_objects_default;
        opp->open = scan_objects_open;
        opp->close = scan_objects_close;
        opp->next = scan_objects_next;

        return brooks_status_ok;
    } else return brooks_status_illegalarg;
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e scan_objects_open(struct brooks_operator_t *self)
{
    if (self->tag == brooks_opp_tag_scan_objects_default) {
        scan_objects_extra_t *extra = (scan_objects_extra_t *) self->extra;

        // TODO: consider cardinality estimation for third parameter
        size_t approx_result_size___upper_bound = 0;
        for (size_t i = 0; i < extra->num_objects; i++) {
            size_t num_elemens = brooks_doc_object_num_elements(extra->objects[i]);
            approx_result_size___upper_bound = (approx_result_size___upper_bound < num_elemens) ?
                                               num_elemens : approx_result_size___upper_bound;
        }


        brooks_cursor_create(&extra->cursor, approx_result_size___upper_bound, extra->pool);
        return brooks_status_ok;
    }
    return brooks_status_badcall;
}

brooks_status_e scan_objects_close(struct brooks_operator_t *self)
{
    if (self->tag != brooks_opp_tag_scan_objects_default) {
        return brooks_status_badcall;
    }
    free (self->extra);
    return brooks_status_ok;
}

const brooks_cursor_t *scan_objects_next(struct brooks_operator_t *self)
{
    if (self->tag != brooks_opp_tag_scan_objects_default) {
        return NULL;
    }

    scan_objects_extra_t *extra = (scan_objects_extra_t *) self->extra;
    brooks_cursor_clear(extra->cursor);

    if (extra->current_object_idx < extra->num_objects) {
        const brooks_object_t *object = extra->objects[extra->current_object_idx++];

        for (brooks_named_entry_t **it = brooks_doc_object_begin(object); it < brooks_doc_object_end(object); it++) {
            const brooks_value_t *value = brooks_doc_named_entry_get_value(*it);
            brooks_cursor_append(extra->cursor, &value, 1);
        }
        return extra->cursor;
    } else return NULL;
}
