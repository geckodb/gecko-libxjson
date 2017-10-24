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

#include <xjson/xjson-query.h>
#include <xjson/xjson-json.h>

typedef struct xjson_query_t {
    xjson_element_t **elements;
    xjson_u64_t num_elements;
} xjson_query_t;

xjson_status_e xjson_query_start(xjson_query_t **query, xjson_pool_t *pool, xjson_object_t *root,
                                 void *capture, xjson_pred_t pred)
{
    xjson_u64_t num_elements_all;
    xjson_element_t **all = xjson_json_fullscan(&num_elements_all, root);
    xjson_query_t *retval = xjson_pool_malloc(pool, sizeof(xjson_query_t));
    retval->num_elements = 0;
    retval->elements = xjson_pool_malloc(pool, num_elements_all * sizeof(xjson_element_t *));
    for (xjson_u64_t i = 0; i < num_elements_all; i++) {
        xjson_element_t *element = all[i];
        if (pred(capture, all[i])) {
            retval->elements[retval->num_elements++] = element;
        }
    }
    *query = retval;
    return xjson_status_ok;
}

xjson_status_e xjson_query_print(FILE *file, const xjson_query_t *query)
{
    if (file && query) {
        for (xjson_u64_t idx = 0; idx < query->num_elements; idx++) {
            xjson_element_t *element = query->elements[idx];
            xjson_element_print(file, element);
            fprintf(file, "\n");
        }
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}