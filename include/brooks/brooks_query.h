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

#ifndef BROOKS_QUERY_H
#define BROOKS_QUERY_H

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>

#include <brooks/brooks.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// C O N F I G
// ---------------------------------------------------------------------------------------------------------------------

#ifndef XJSON_RESULT_CAPACITY_DEFAULT
    #define XJSON_RESULT_CAPACITY_DEFAULT                      5
#endif

#ifndef XJSON_QUERY_FILTERS_CAPACITY_DEFAULT
    #define XJSON_QUERY_FILTERS_CAPACITY_DEFAULT               5
#endif

#ifndef XJSON_QUERY_TERMINATORS_CAPACITY_DEFAULT
    #define XJSON_QUERY_TERMINATORS_CAPACITY_DEFAULT           5
#endif

// ---------------------------------------------------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_object_t           brooks_object_t;

typedef struct brooks_pool_t             brooks_pool_t;

typedef struct brooks_element_t          brooks_element_t;

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_query_t            brooks_query_t;

typedef struct brooks_result_t           brooks_result_t;

typedef enum {
    brooks_entry_kind_key_value_pair,
    brooks_entry_kind_single_value,
    brooks_entry_kind_any
} brooks_entry_kind_e;

typedef enum {
    brooks_traversal_breadth_first, brooks_traversal_depth_first
} brooks_traversal_policy_e;

typedef bool (*brooks_pred_val_type_t)(void *capture, const brooks_type_e type);

typedef bool (*brook_pred_integer_t)(void *capture, const uint64_t *integer);

typedef bool (*brooks_pred_decimal_t)(void *capture, const double *decimal);

typedef bool (*brooks_pred_string_t)(void *capture, const char **string);

typedef bool (*xjson_pred_boolean_t)(void *capture, const bool *boolean);

typedef struct brooks_filter_t brooks_filter_t;

typedef enum brooks_selector_e
{
    brooks_selector_include, brooks_selector_exclude
} brooks_selector_e;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_query_create(brooks_query_t **query, brooks_pool_t *pool);

brooks_status_e brooks_query_add_path_filter(brooks_query_t *query, const brooks_filter_t *filter);

brooks_status_e brooks_query_add_terminator(brooks_query_t *query, const brooks_filter_t *filter);

brooks_status_e brooks_query_execute(brooks_result_t **result, brooks_pool_t *pool, const brooks_query_t *query,
                                     const brooks_object_t *root, brooks_traversal_policy_e policy);

brooks_status_e brooks_result_print(FILE *file, const brooks_result_t *result);

brooks_status_e brooks_filter_create(brooks_filter_t **filter, brooks_pool_t *pool,
                                     size_t min_depth, size_t max_depth);

brooks_status_e brooks_filter_set_prop_key_name(brooks_filter_t *filter, brooks_pred_string_t pred);

brooks_status_e brooks_filter_set_prop_index(brooks_filter_t *filter, brook_pred_integer_t pred);

brooks_status_e brooks_filter_set_value_type(brooks_filter_t *filter, brooks_pred_val_type_t pred);

brooks_status_e brooks_filter_set_entry_kind(brooks_filter_t *filter, brooks_entry_kind_e pred);

brooks_status_e brooks_filter_set_value_int(brooks_filter_t *filter, brook_pred_integer_t pred);

brooks_status_e brooks_filter_set_value_dec(brooks_filter_t *filter, brooks_pred_decimal_t pred);

brooks_status_e brooks_filter_set_value_str(brooks_filter_t *filter, brooks_pred_string_t pred);

brooks_status_e brooks_filter_set_value_bool(brooks_filter_t *filter, xjson_pred_boolean_t pred);

brooks_status_e brooks_filter_set_array_num_elem_min(brooks_filter_t *filter, brook_pred_integer_t pred);

brooks_status_e brooks_filter_set_array_num_elem_max(brooks_filter_t *filter, brook_pred_integer_t pred);

brooks_status_e brooks_filter_set_object_num_elem_min(brooks_filter_t *filter, brook_pred_integer_t pred);

brooks_status_e brooks_filter_set_object_num_elem_max(brooks_filter_t *filter, brook_pred_integer_t pred);


#ifdef __cplusplus
}
#endif

#endif //BROOKS_QUERY_H
