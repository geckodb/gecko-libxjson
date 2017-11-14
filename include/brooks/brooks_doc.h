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

#include <stdbool.h>
#include <stdint.h>

#include <brooks/brooks.h>
#include <brooks/brooks_pool.h>


#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------------------------------------------------
// C O N F I G
// ---------------------------------------------------------------------------------------------------------------------

#ifndef BOOKS_DEFAULT_CAPACITY_VALUE
    #define BOOKS_DEFAULT_CAPACITY_VALUE                    300
#endif

#ifndef BROOKS_OBJECT_CAPACITY
    #define BROOKS_OBJECT_CAPACITY                          BOOKS_DEFAULT_CAPACITY_VALUE
#endif
#ifndef BROOKS_ARRAY_CAPACITY
    #define BROOKS_ARRAY_CAPACITY                           BOOKS_DEFAULT_CAPACITY_VALUE
#endif

// ---------------------------------------------------------------------------------------------------------------------
// F O R W A R D   D E C L A R A T I O N S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct brooks_object_t         brooks_object_t;

typedef struct brooks_array_t          brooks_array_t;

typedef struct brooks_unnamed_entry_t  brooks_unnamed_entry_t;

typedef struct brooks_named_entry_t    brooks_named_entry_t;

typedef struct brooks_element_t        brooks_element_t;

typedef struct brooks_value_t          brooks_value_t;

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_doc_create(brooks_object_t **doc, brooks_pool_t *pool);

brooks_status_e brooks_doc_parse(brooks_object_t **doc, const char *text);

brooks_status_e brooks_doc_print(FILE *file, const brooks_object_t *json);

brooks_status_e brooks_doc_add_boolean(brooks_object_t *parent, const char *key, const bool *data);

brooks_status_e brooks_doc_add_decimal(brooks_object_t *parent, const char *key, const double *data);

brooks_status_e brooks_doc_add_integer(brooks_object_t *parent, const char *key, const uint64_t *data);

brooks_status_e brooks_doc_add_string(brooks_object_t *parent, const char *key, const char *data);

brooks_status_e brooks_doc_add_null(brooks_object_t *parent, const char *key);

brooks_status_e brooks_doc_add_object(brooks_object_t **object, brooks_object_t *parent, const char *key);

brooks_status_e brooks_doc_add_array(brooks_array_t **array, brooks_object_t *parent, brooks_type_e type,
                                     const char *key);

brooks_status_e brooks_doc_add_value(brooks_array_t *parent, const void *data);

brooks_named_entry_t **brooks_doc_object_begin(const brooks_object_t *object);

brooks_named_entry_t **brooks_doc_object_end(const brooks_object_t *object);

size_t brooks_doc_object_num_elements(const brooks_object_t *object);

brooks_status_e brooks_doc_array_add_object(brooks_object_t **object, brooks_array_t *parent);

brooks_status_e brooks_doc_array_add_array(brooks_array_t **array, brooks_type_e type, brooks_array_t *parent);

size_t brooks_doc_array_get_length(const brooks_array_t *array);

brooks_unnamed_entry_t **brooks_doc_array_begin(const brooks_array_t *array);

brooks_unnamed_entry_t **brooks_doc_array_end(const brooks_array_t *array);

brooks_status_e brooks_doc_array_print(FILE *file, const brooks_array_t *array);

brooks_status_e brooks_doc_element_has_key(brooks_element_t *element);

brooks_status_e brooks_doc_element_get(const char **key, const brooks_value_t **value, brooks_element_t *element);

brooks_status_e brooks_doc_element_get_type(brooks_type_e *type, const brooks_element_t *element);

brooks_status_e brooks_doc_element_print(FILE *file, brooks_element_t *element);

brooks_status_e brooks_doc_value_get_type(brooks_type_e *type, const brooks_value_t *value);

uint64_t brooks_doc_value_as_integer(const brooks_value_t *value);

double brooks_doc_value_as_double(const brooks_value_t *value);

const char *brooks_doc_value_as_string(const brooks_value_t *value);

brooks_array_t *brooks_doc_value_as_array(const brooks_value_t *value);

bool brooks_doc_value_as_boolean(const brooks_value_t *value);

const brooks_object_t *brooks_doc_value_as_object(const brooks_value_t *value);

brooks_status_e brooks_doc_value_print(FILE *file, const brooks_value_t *value);

const brooks_value_t *brooks_doc_named_entry_get_value(const brooks_named_entry_t *entry);

const brooks_value_t *brooks_doc_unnamed_entry_get_value(const brooks_unnamed_entry_t *entry);

const char *brooks_doc_type_str(const brooks_type_e type);

brooks_pool_t *brooks_doc_get_pool(const brooks_object_t *object);


#ifdef __cplusplus
}
#endif

#endif // LIBXJSON_H
