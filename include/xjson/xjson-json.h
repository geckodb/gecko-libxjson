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

#ifndef LIBXJSON_H
#define LIBXJSON_H

// ---------------------------------------------------------------------------------------------------------------------
// I N C L U D E S
// ---------------------------------------------------------------------------------------------------------------------

#include <xjson/xjson.h>
#include <xjson/xjson-pool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// C O N F I G
// ---------------------------------------------------------------------------------------------------------------------

#ifndef XJSON_DEFAULT_CAPACITY_VALUE
    #define XJSON_DEFAULT_CAPACITY_VALUE                    15
#endif

#ifndef XJSON_JSON_ENTRIES_CAPACITY
    #define XJSON_JSON_ENTRIES_CAPACITY                     XJSON_DEFAULT_CAPACITY_VALUE
#endif
#ifndef XJSON_ARRAY_CAPACITY
    #define XJSON_ARRAY_CAPACITY                            XJSON_DEFAULT_CAPACITY_VALUE
#endif

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef enum
{
    xjson_none,

    xjson_object,
    xjson_array,

    xjson_number_integer,
    xjson_number_double,
    xjson_string,
    xjson_boolean,

    xjson_null
}                                     xjson_type_e;

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
// I N T E R F A C E   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

xjson_status_e xjson_json_create(xjson_object_t **json, xjson_pool_t *pool);

xjson_status_e xjson_json_parse(xjson_object_t **json, const char *text);

xjson_status_e xjson_json_print(FILE *file, const xjson_object_t *json);

xjson_status_e xjson_json_add_property(xjson_object_t *parent, xjson_type_e type, const char *key, const void *data);

xjson_status_e xjson_json_add_object(xjson_object_t **object, xjson_object_t *parent, const char *key);

xjson_status_e xjson_json_add_array(xjson_array_t **array, xjson_object_t *parent, xjson_type_e type, const char *key);

xjson_status_e xjson_array_add_value(xjson_array_t *parent, const void *data);

xjson_status_e xjson_array_add_object(xjson_object_t **object, xjson_array_t *parent);

xjson_status_e xjson_array_add_array(xjson_array_t **array, xjson_type_e type, xjson_array_t *parent);

xjson_status_e xjson_array_print(FILE *file, const xjson_array_t *array);

xjson_status_e xjson_scan(xjson_element_t **element, const xjson_object_t *object);

xjson_status_e xjson_element_has_key(xjson_element_t *element);

xjson_status_e xjson_element_get(const char **key, const xjson_value_t **value, xjson_element_t *element);

xjson_status_e xjson_value_get_type(xjson_type_e *type, const xjson_value_t *value);

const char *xjson_type_str(const xjson_type_e type);

/*


xjson_status_e  xjson_json_remove(xjson_object_t *json, const xjson_named_entry_t *entry);

xjson_status_e  xjson_json_query(xjson_json_cursor_t **cursor, const xjson_object_t *json,
                                 const void *capture, xjson_entry_pred_t pred);

xjson_status_e  xjson_query_next(xjson_json_cursor_t *cursor);

xjson_status_e  xjson_query_rewind(xjson_json_cursor_t *cursor);

xjson_status_e  xjson_query_close(xjson_json_cursor_t *cursor);

xjson_status_e  xjson_query_length(xjson_size_t **length, const xjson_json_cursor_t *cursor);

xjson_status_e  xjson_query_value_get_type(xjson_type_e *type, xjson_json_cursor_t *cursor);

xjson_status_e  xjson_query_get_context(xjson_object_t **context, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_get_key(const char **key, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_is_null(const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_is_none(const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_has_value(const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_get_index(xjson_size_t **idx, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_object(xjson_object_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_array(xjson_array_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_number_int(xjson_u64_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_number_double(xjson_double_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_number_string(const char **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_query_value_get_number_boolean(xjson_bool_t **value, const xjson_named_entry_t *entry);



xjson_status_e  xjson_entry_get_type(xjson_type_e *type, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_get_context(xjson_object_t **context, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_get_key(const char **key, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_is_null(const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_is_none(const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_has_value(const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_object(xjson_object_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_array(xjson_array_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_number_int(xjson_u64_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_number_double(xjson_double_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_number_string(const char **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_entry_value_get_number_boolean(xjson_bool_t **value, const xjson_named_entry_t *entry);

xjson_status_e  xjson_array_get_length(xjson_size_t **length, const xjson_array_t *array);



xjson_status_e  xjson_array_query(xjson_array_t *array, const void *capture, xjson_array_pred_t pred);*/

#ifdef __cplusplus
}
#endif

#endif // LIBXJSON_H
