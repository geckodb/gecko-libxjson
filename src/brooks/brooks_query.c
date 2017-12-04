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

#include <brooks/brooks.h>
#include <brooks/brooks_query.h>
#include <brooks/brooks_doc.h>
#include <brooks/brooks_misc.h>
#include <string.h>

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_result_t *result_create(brooks_pool_t *pool);

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E   D E F I N I T I O N
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_query_t {
    brooks_vector_t                      filters;
    brooks_vector_t                      terminators;
} brooks_query_t;

typedef struct brooks_result_t
{
    brooks_vector_t                      data;
} brooks_result_t;

typedef struct brooks_filter_t
{
    size_t                              min_depth;
    size_t                              max_depth;

    brooks_pred_string_t                 pred_prop_key_name;
    brook_pred_integer_t                pred_prop_index;
    brooks_pred_val_type_t               pred_value_type;
    brooks_entry_kind_e                  pred_entry_kind;
    brook_pred_integer_t                pred_value_int;
    brooks_pred_decimal_t                pred_value_dec;
    brooks_pred_string_t                 pred_value_str;
    xjson_pred_boolean_t                pred_value_bool;
    brook_pred_integer_t                pred_array_num_elem_min;
    brook_pred_integer_t                pred_array_num_elem_max;
    brook_pred_integer_t                pred_object_num_elem_min;
    brook_pred_integer_t                pred_object_num_elem_max;
} brooks_filter_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

/*brooks_status_e xjson_query_start(brooks_query_t **query, brooks_pool_t *pool, brooks_object_t *root,
                                 void *capture, xjson_pred_t pred)
{
    xjson_u64_t num_elements_all;
    brooks_element_t **all = xjson_json_fullscan(&num_elements_all, root);
    brooks_query_t *retval = brooks_pool_malloc(pool, sizeof(brooks_query_t));
    retval->num_elements_data_list = 0;
    retval->elements = brooks_pool_malloc(pool, num_elements_all * sizeof(brooks_element_t *));
    for (xjson_u64_t i = 0; i < num_elements_all; i++) {
        brooks_element_t *element = all[i];
        if (pred(capture, all[i])) {
            retval->elements[retval->num_elements_data_list++] = element;
        }
    }
    *query = retval;
    return brooks_status_ok;
}*/

brooks_status_e brooks_query_create(brooks_query_t **query, brooks_pool_t *pool)
{
    if (pool) {
        brooks_query_t *retval = brooks_pool_malloc(pool, sizeof(brooks_query_t));
        if (retval) {
            INIT_VECTOR(retval->filters, sizeof(brooks_filter_t *), XJSON_QUERY_FILTERS_CAPACITY_DEFAULT, pool);
            INIT_VECTOR(retval->terminators, sizeof(brooks_filter_t *), XJSON_QUERY_TERMINATORS_CAPACITY_DEFAULT, pool);
        }
        return ((retval != NULL && retval->filters.base != NULL && retval->terminators.base != NULL) ?
                    brooks_status_ok : brooks_status_malloc_err);
    } else return brooks_status_nullptr;
}

brooks_status_e brooks_query_add_path_filter(brooks_query_t *query, const brooks_filter_t *filter)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_query_add_terminator(brooks_query_t *query, const brooks_filter_t *filter)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_query_execute(brooks_result_t **result, brooks_pool_t *pool, const brooks_query_t *query,
                                     const brooks_object_t *root, brooks_traversal_policy_e policy)
{

   /* size_t num_elements_all;
    brooks_element_t **all = NULL; //brooks_querfullscan(&num_elements_all, root);
    brooks_result_t *retval = result_create(pool);
    for (size_t i = 0; i < num_elements_all; i++) {
        brooks_element_t *element = all[i];
        //if (pred(capture, all[i])) {
        retval->data.base = brooks_misc_autoresize(retval->data.base, retval->data.element_size,
                                                   &retval->data.element_size, &retval->data.capacity_data_list);
      //  memcpy(retval->data.base + (retval->data.num_elements_data_list++ * retval->data.element_size),
    //           element, sizeof());

         //   retval->elements[retval->num_elements_data_list++] = element;
        //}
    }
    *result = retval;*/
    return brooks_status_ok;
}

brooks_status_e brooks_result_print(FILE *file, const brooks_result_t *result)
{
    if (file && result) {
        for (size_t idx = 0; idx < result->data.num_elements; idx++) {
            brooks_element_t *element = result->data.base + (idx * result->data.element_size);
            brooks_doc_element_print(file, element);
            fprintf(file, "\n");
        }
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

brooks_status_e brooks_filter_create(brooks_filter_t **filter, brooks_pool_t *pool,
                                     size_t min_depth, size_t max_depth)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_prop_key_name(brooks_filter_t *filter, brooks_pred_string_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_prop_index(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_value_type(brooks_filter_t *filter, brooks_pred_val_type_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_entry_kind(brooks_filter_t *filter, brooks_entry_kind_e pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_value_int(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_value_dec(brooks_filter_t *filter, brooks_pred_decimal_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_value_str(brooks_filter_t *filter, brooks_pred_string_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_value_bool(brooks_filter_t *filter, xjson_pred_boolean_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_array_num_elem_min(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_array_num_elem_max(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_object_num_elem_min(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

brooks_status_e brooks_filter_set_object_num_elem_max(brooks_filter_t *filter, brook_pred_integer_t pred)
{
    return brooks_status_interalerr;
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_result_t *result_create(brooks_pool_t *pool)
{
    brooks_result_t *result;
    if ((result = brooks_pool_malloc(pool, sizeof(brooks_result_t))) != NULL) {
        INIT_VECTOR(result->data, sizeof(brooks_element_t *), XJSON_RESULT_CAPACITY_DEFAULT, pool);
        return (result->data.base != NULL ? result : NULL);
    }
    return NULL;
}