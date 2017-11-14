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
#include <memory.h>

#include <brooks/brooks.h>
#include <brooks/brooks_doc.h>
#include <brooks/brooks_misc.h>
#include <math.h>

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef struct entry_desc_t
{
    union {
        brooks_named_entry_t         *named_entry;
        brooks_unnamed_entry_t       *unnamed_entry;
    } context;
    brooks_entry_type_e               context_type;
} entry_desc_t;

typedef struct brooks_object_t
{
    entry_desc_t                  context_desc;
    brooks_named_entry_t         **entries;
    size_t                        num_entries;
    size_t                        capacity;
    size_t                        idx;
    brooks_pool_t                 *pool;
} brooks_object_t;

typedef struct brooks_array_t
{
    entry_desc_t             context_desc;
    size_t                         num_entries;
    size_t                         capacity;
    brooks_type_e                  type;
    brooks_unnamed_entry_t       **entries;
} brooks_array_t;

typedef struct  brooks_unnamed_entry_t
{
    brooks_array_t                *context;
    size_t                        idx;
    brooks_value_t                *value;

} brooks_unnamed_entry_t;

typedef struct brooks_named_entry_t
{
    brooks_object_t               *context;
    char                         *key;
    brooks_value_t                *value;
} brooks_named_entry_t;

typedef struct brooks_element_t {
    entry_desc_t            entry;
    size_t                        idx;
} brooks_element_t;

typedef struct brooks_value_t
{
    entry_desc_t            context_desc;
    brooks_type_e                  type;
    union {
        brooks_array_t            *array;
        brooks_object_t           *object;
        uint64_t                  integer;
        double                    decimal;
        char                     *string;
        bool                      boolean;
    };
} brooks_value_t;

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------
static brooks_status_e property_add(brooks_object_t *parent, brooks_type_e type, const char *key,
                                    const void *data);

static brooks_status_e value_set(brooks_value_t *value, brooks_pool_t *pool, brooks_type_e type, const void *data);

static brooks_object_t *json_create(brooks_pool_t *pool, brooks_entry_type_e parent_type, void *parent_ptr);
static brooks_status_e json_autoresize(brooks_object_t *object);
static brooks_status_e array_autoresize(brooks_array_t *array);
static void json_add_entry(brooks_object_t *object, brooks_named_entry_t *entry);
static brooks_status_e json_add_complex(brooks_object_t **object, brooks_array_t **array, brooks_object_t *parent,
                                       const char *key, brooks_type_e complex_type, brooks_type_e array_type);
static brooks_value_t *value_create(brooks_pool_t *pool, brooks_type_e type, brooks_entry_type_e context,
                                   void *parent_ptr);
static brooks_named_entry_t *entry_create(brooks_object_t *object, brooks_type_e type, const char *key);
static brooks_array_t *array_create(brooks_type_e type, brooks_entry_type_e context, void *parent_ptr);
static brooks_pool_t *context_get_pool(entry_desc_t *desc);
static brooks_unnamed_entry_t *array_entry_create(brooks_pool_t *pool, brooks_array_t *context);
static brooks_element_t *element_create(brooks_pool_t *pool, brooks_entry_type_e entry_type, void *entry, size_t idx);

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

brooks_status_e brooks_doc_create(brooks_object_t **json, brooks_pool_t *pool)
{
    *json = json_create(pool, brooks_entry_type_named_entry, BROOKS_OBJECT_ROOT);
    return (*json != NULL ? brooks_status_ok : (pool != NULL ? brooks_status_failed : brooks_status_nopool));
}

brooks_status_e brooks_doc_parse(brooks_object_t **json, const char *text)
{
    // TODO:...
    return brooks_status_interalerr;
}

brooks_status_e brooks_doc_print(FILE *file, const brooks_object_t *json)
{
    if (file && json) {
        fprintf(file, "{ ");
        for (size_t i = 0; i < json->num_entries; i++) {
            brooks_named_entry_t *entry = json->entries[i];
            printf("\"%s\": ", entry->key);
            switch (entry->value->type) {
                case brooks_type_object:
                    brooks_doc_print(file, entry->value->object);
                    break;
                case brooks_type_array:
                    brooks_doc_array_print(file, entry->value->array);
                    break;
                case brooks_type_number_integer:
                    fprintf(file, "%lld", entry->value->integer);
                    break;
                case brooks_type_number_double:
                    fprintf(file, "%f", entry->value->decimal);
                    break;
                case brooks_type_string:
                    fprintf(file, "\"%s\"", entry->value->string);
                    break;
                case brooks_type_boolean:
                    fprintf(file, "\"%s\"", (entry->value->boolean ? "true" : "false"));
                    break;
                case brooks_type_null:
                    fprintf(file, "null");
                    break;
                default:
                    fprintf(file, "(unknown)");
                    break;
            }
            fprintf(file, "%s ", (i + 1 < json->num_entries ? "," : ""));
        }
        fprintf(file, "}");
    }
    return brooks_status_ok;
}

brooks_status_e brooks_doc_add_boolean(brooks_object_t *parent, const char *key, const bool *data)
{
    return property_add(parent, brooks_type_boolean, key, data);
}

brooks_status_e brooks_doc_add_decimal(brooks_object_t *parent, const char *key, const double *data)
{
    return property_add(parent, brooks_type_number_double, key, data);
}

brooks_status_e brooks_doc_add_integer(brooks_object_t *parent, const char *key, const uint64_t *data)
{
    return property_add(parent, brooks_type_number_integer, key, data);
}

brooks_status_e brooks_doc_add_string(brooks_object_t *parent, const char *key, const char *data)
{
    return property_add(parent, brooks_type_string, key, data);
}


brooks_status_e brooks_doc_add_null(brooks_object_t *parent, const char *key)
{
    return property_add(parent, brooks_type_null, key, NULL);
}

brooks_status_e brooks_doc_add_object(brooks_object_t **object, brooks_object_t *parent, const char *key)
{
    return json_add_complex(object, NULL, parent, key, brooks_type_object, 0);
}

brooks_status_e brooks_doc_add_array(brooks_array_t **array, brooks_object_t *parent, brooks_type_e type, const char *key)
{
    return json_add_complex(NULL, array, parent, key, brooks_type_array, type);
}

brooks_status_e brooks_doc_add_value(brooks_array_t *parent, const void *data)
{
    brooks_status_e status;
    if (parent != NULL && data != NULL) {
        brooks_pool_t *pool = context_get_pool(&parent->context_desc);
        brooks_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = value_create(pool, parent->type, brooks_entry_type_unnamed_entry, parent);
        if (((status = value_set(entry->value, pool, parent->type, data)) == brooks_status_ok) &&
                (status = array_autoresize(parent)) == brooks_status_ok) {
            parent->entries[parent->num_entries++] = entry;
            return brooks_status_ok;
        } else {
            return status;
        }
    } else return brooks_status_nullptr;
}

brooks_named_entry_t **brooks_doc_object_begin(const brooks_object_t *object)
{
    return (object ? object->entries : NULL);
}

brooks_named_entry_t **brooks_doc_object_end(const brooks_object_t *object)
{
    return (object ? (object->entries + object->num_entries) : NULL);
}

size_t brooks_doc_object_num_elements(const brooks_object_t *object)
{
    return (object ? (object->num_entries) : 0);
}

brooks_status_e brooks_doc_array_add_object(brooks_object_t **object, brooks_array_t *parent)
{
    brooks_status_e status;
    if (parent != NULL) {
        brooks_pool_t *pool = context_get_pool(&parent->context_desc);
        status = array_autoresize(parent);
        brooks_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = brooks_pool_malloc(pool, sizeof(brooks_value_t));
        entry->value->object = json_create(pool, brooks_entry_type_unnamed_entry, entry);
        entry->value->type = parent->type;
        parent->entries[parent->num_entries++] = entry;
        *object = entry->value->object;
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

brooks_status_e brooks_doc_array_add_array(brooks_array_t **array, brooks_type_e type, brooks_array_t *parent)
{
    brooks_status_e status;
    if (parent != NULL) {
        brooks_pool_t *pool = context_get_pool(&parent->context_desc);
        status = array_autoresize(parent);
        brooks_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = brooks_pool_malloc(pool, sizeof(brooks_value_t));
        entry->value->array = array_create(type, brooks_entry_type_unnamed_entry, entry);
        parent->entries[parent->num_entries++] = entry;
        *array = entry->value->array;
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

size_t brooks_doc_array_get_length(const brooks_array_t *array)
{
    return (array ? array->num_entries : 0);
}

brooks_unnamed_entry_t **brooks_doc_array_begin(const brooks_array_t *array)
{
    return (array ? array->entries : NULL);
}

brooks_unnamed_entry_t **brooks_doc_array_end(const brooks_array_t *array)
{
    return (array ? (array->entries + array->num_entries) : NULL);
}

brooks_status_e brooks_doc_array_print(FILE *file, const brooks_array_t *array)
{
    if (file && array) {
        printf("[ ");
        for (size_t i = 0; i < array->num_entries; i++) {
            brooks_unnamed_entry_t *entry = array->entries[i];
            switch (array->type) {
                case brooks_type_object:
                    brooks_doc_print(file, entry->value->object);
                    break;
                case brooks_type_array:
                    brooks_doc_array_print(file, entry->value->array);
                    break;
                case brooks_type_number_integer:
                    fprintf(file, "%lld", entry->value->integer);
                    break;
                case brooks_type_number_double:
                    fprintf(file, "%f", entry->value->decimal);
                    break;
                case brooks_type_string:
                    fprintf(file, "\"%s\"", entry->value->string);
                    break;
                case brooks_type_boolean:
                    fprintf(file, "\"%s\"", (entry->value->boolean ? "true" : "false"));
                    break;
                case brooks_type_null:
                    fprintf(file, "null");
                    break;
                default:
                    fprintf(file, "(unknown)");
                    break;
            }
            fprintf(file, "%s ", (i + 1 < array->num_entries ? "," : ""));
        }
        printf("]");
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

brooks_element_t **brooks_doc_fullscan(size_t *num_elements, const brooks_object_t *object)
{
    *num_elements = object->num_entries;
    brooks_element_t **retval = NULL;
    if (object->num_entries > 0) {
        retval = brooks_pool_malloc(object->pool, object->num_entries * sizeof(brooks_element_t *));
        for (size_t idx = 0; idx < object->num_entries; idx++) {
            brooks_named_entry_t *entry = object->entries[idx];
            brooks_element_t *result = element_create(object->pool, brooks_entry_type_named_entry, entry, idx);
            retval[idx] = result;
        }
    }
    return retval;
}

brooks_status_e brooks_doc_element_has_key(brooks_element_t *element)
{
    return ((element != NULL && element->entry.context_type == brooks_entry_type_named_entry) ?
            brooks_status_true : brooks_status_false);
}

brooks_status_e brooks_doc_element_get(const char **key, const brooks_value_t **value, brooks_element_t *element)
{
    if (element) {
        if (key && brooks_doc_element_has_key(element)) {
            *key = element->entry.context.named_entry->key;
        } else {
            return brooks_status_false;
        }
        if (value) {
            switch (element->entry.context_type) {
                case brooks_entry_type_named_entry:
                    *value = element->entry.context.named_entry->value;
                    break;
                case brooks_entry_type_unnamed_entry:
                    *value = element->entry.context.unnamed_entry->value;
                    break;
                default:
                    return brooks_status_interalerr;
            }
        }
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

brooks_status_e brooks_doc_element_get_type(brooks_type_e *type, const brooks_element_t *element)
{
    switch (element->entry.context_type) {
        case brooks_entry_type_named_entry:
            *type = element->entry.context.named_entry->value->type;
            break;
        case brooks_entry_type_unnamed_entry:
            *type = element->entry.context.unnamed_entry->context->type;
            break;
        default:
            return brooks_status_interalerr;
    }
    return brooks_status_ok;
}

brooks_status_e brooks_doc_element_print(FILE *file, brooks_element_t *element)
{
    if (file && element) {
        fprintf(file, "element[%zud]", element->idx);
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

brooks_status_e brooks_doc_value_get_type(brooks_type_e *type, const brooks_value_t *value)
{
    if (type && value) {
        *type = value->type;
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

uint64_t brooks_doc_value_as_integer(const brooks_value_t *value)
{
    return (value ? value->integer : 0);
}

double brooks_doc_value_as_double(const brooks_value_t *value)
{
    return (value ? value->decimal : NAN);
}

const char *brooks_doc_value_as_string(const brooks_value_t *value)
{
    return (value ? value->string : NULL);
}

brooks_array_t *brooks_doc_value_as_array(const brooks_value_t *value)
{
    return (value ? value->array : NULL);
}

bool brooks_doc_value_as_boolean(const brooks_value_t *value)
{
    return (value ? value->boolean : 0);
}

const brooks_object_t *brooks_doc_value_as_object(const brooks_value_t *value)
{
    return (value ? value->object : NULL);
}

brooks_status_e brooks_doc_value_print(FILE *file, const brooks_value_t *value)
{
    if (file && value) {
        fprintf(file, "{ \"type\": \"%s\", \"address\": %p }", brooks_doc_type_str(value->type), value);
        return brooks_status_ok;
    } else return brooks_status_nullptr;
}

const brooks_value_t *brooks_doc_named_entry_get_value(const brooks_named_entry_t *entry)
{
    return (entry != NULL ? entry->value : NULL);
}

const brooks_value_t *brooks_doc_unnamed_entry_get_value(const brooks_unnamed_entry_t *entry)
{
    return (entry != NULL ? entry->value : NULL);
}

const char *brooks_doc_type_str(const brooks_type_e type)
{
    switch (type) {
        case brooks_type_none:            return "none";
        case brooks_type_object:          return "object";
        case brooks_type_array:           return "array";
        case brooks_type_number_integer:  return "number_u64";
        case brooks_type_number_double:   return "number_double";
        case brooks_type_string:          return "string";
        case brooks_type_boolean:         return "boolean";
        case brooks_type_null:            return "null";
        default: return NULL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static brooks_status_e property_add(brooks_object_t *parent, brooks_type_e type, const char *key, const void *data)
{
    if (parent != NULL && (type == brooks_type_null || data != NULL)) {
        brooks_named_entry_t *entry = entry_create(parent, type, key);
        if (entry != NULL && (json_autoresize(parent) == brooks_status_ok)) {
            brooks_status_e status = value_set(entry->value, parent->pool, type, data);
            if (status == brooks_status_ok) {
                json_add_entry(parent, entry);
                return brooks_status_ok;
            } else {
                return status;
            }
        } else return brooks_status_pmalloc_err;
    } else return brooks_status_nullptr;
}

static brooks_status_e value_set(brooks_value_t *value, brooks_pool_t *pool, brooks_type_e type, const void *data)
{
    switch (type) {
        case brooks_type_number_integer:
            memcpy(&value->integer, data, sizeof(uint64_t));
            break;
        case brooks_type_number_double:
            memcpy(&value->decimal, data, sizeof(double));
            break;
        case brooks_type_string:
            value->string = brooks_misc_strdup(pool, data);
            break;
        case brooks_type_boolean:
            memcpy(&value->boolean, data, sizeof(bool));
            break;
        case brooks_type_null:
            break;
        case brooks_type_object:
        case brooks_type_array:
            return brooks_status_wrongusage;
        default:
            return brooks_status_notype;
    }
    return brooks_status_ok;
}

static brooks_object_t *json_create(brooks_pool_t *pool, brooks_entry_type_e parent_type, void *parent_ptr)
{
    brooks_object_t *retval = NULL;
    if ((pool != NULL) &&
        ((retval = brooks_pool_malloc(pool, sizeof(brooks_object_t))) != NULL) &&
        ((retval->entries = brooks_pool_malloc(pool, BROOKS_OBJECT_CAPACITY * sizeof(brooks_named_entry_t *))) != NULL)) {
        retval->capacity = BROOKS_OBJECT_CAPACITY;
        retval->idx = retval->num_entries = 0;
        retval->context_desc.context_type = parent_type;
        switch (parent_type) {
            case brooks_entry_type_named_entry:
                retval->context_desc.context.named_entry = parent_ptr;
                break;
            case brooks_entry_type_unnamed_entry:
                retval->context_desc.context.unnamed_entry = parent_ptr;
                break;
            default: return NULL;
        }
        retval->pool = pool;
    }
    return retval;
}

static brooks_status_e json_autoresize(brooks_object_t *object)
{
    object->entries = brooks_misc_pooled_autoresize(object->pool, object->entries, sizeof(brooks_named_entry_t *),
                                                    object->num_entries, &object->capacity, 1);
    return (object->entries != NULL ? brooks_status_ok : brooks_status_pmalloc_err);
}

static brooks_status_e array_autoresize(brooks_array_t *array)
{
    array->entries = brooks_misc_pooled_autoresize(context_get_pool(&array->context_desc), array->entries,
                                                   sizeof(brooks_unnamed_entry_t *), array->num_entries,
                                                   &array->capacity, 1);
    return (array->entries != NULL ? brooks_status_ok : brooks_status_pmalloc_err);
}

static void json_add_entry(brooks_object_t *object, brooks_named_entry_t *entry)
{
    object->entries[object->num_entries++] = entry;
}

static brooks_status_e json_add_complex(brooks_object_t **object, brooks_array_t **array, brooks_object_t *parent,
                                       const char *key, brooks_type_e complex_type, brooks_type_e array_type)
{
    brooks_object_t *retval_object;
    brooks_array_t *retval_array;
    brooks_named_entry_t *entry;
    brooks_status_e status;

    if (complex_type != brooks_type_array && complex_type != brooks_type_object) {
        return brooks_status_interalerr;
    } else {
        if (parent != NULL && key != NULL && ((entry = entry_create(parent, complex_type, key)) != NULL) &&
            (((complex_type != brooks_type_object) ||
                    ((retval_object = json_create(parent->pool, brooks_entry_type_named_entry, entry)) != NULL)) &&
             ((complex_type != brooks_type_array) ||
                     ((retval_array = array_create(array_type, brooks_entry_type_named_entry, entry)) != NULL)))) {
            if ((status = json_autoresize(parent)) == brooks_status_ok) {
                json_add_entry(parent, entry);
                if (complex_type == brooks_type_object) {
                    entry->value->object = retval_object;
                    *object = retval_object;
                } else {
                    entry->value->array = retval_array;
                    *array = retval_array;
                }
                return brooks_status_ok;
            } return status;
        } else return brooks_status_nullptr;
    }
}

static brooks_value_t *value_create(brooks_pool_t *pool, brooks_type_e type, brooks_entry_type_e context,
                                   void *parent_ptr)
{
    brooks_value_t *value = brooks_pool_malloc(pool, sizeof(brooks_value_t));
    value->type = type;
    value->context_desc.context_type = context;
    switch (context) {
        case brooks_entry_type_named_entry:
            value->context_desc.context.named_entry = parent_ptr;
            break;
        case brooks_entry_type_unnamed_entry:
            value->context_desc.context.unnamed_entry = parent_ptr;
            break;
        default: return NULL;
    }
    return value;
}

static brooks_named_entry_t *entry_create(brooks_object_t *object, brooks_type_e type, const char *key)
{
    brooks_named_entry_t *entry = brooks_pool_malloc(object->pool, sizeof(brooks_named_entry_t));
    brooks_value_t *value = value_create(object->pool, type, brooks_entry_type_named_entry, entry);
    entry->context = object;
    entry->key = brooks_misc_strdup(object->pool, key);
    entry->value = value;
    return entry;
}

static brooks_array_t *array_create(brooks_type_e type, brooks_entry_type_e context, void *parent_ptr)
{
    entry_desc_t context_desc;
    switch (context) {
        case brooks_entry_type_unnamed_entry:
            context_desc.context.unnamed_entry = parent_ptr;
            break;
        case brooks_entry_type_named_entry:
            context_desc.context.named_entry = parent_ptr;
            break;
        default: return NULL;
    }
    context_desc.context_type = context;
    brooks_pool_t *pool = context_get_pool(&context_desc);
    brooks_array_t *retval = brooks_pool_malloc(pool, sizeof(brooks_array_t));
    retval->context_desc = context_desc;
    retval->num_entries = 0;
    retval->type = type;
    retval->capacity = BROOKS_ARRAY_CAPACITY;
    retval->entries = brooks_pool_malloc(pool, BROOKS_ARRAY_CAPACITY * sizeof(brooks_unnamed_entry_t *));
    return retval;
}

static brooks_pool_t *context_get_pool(entry_desc_t *desc)
{
    switch (desc->context_type) {
        case brooks_entry_type_named_entry:
            return desc->context.named_entry->context->pool;
        case brooks_entry_type_unnamed_entry:
            return context_get_pool(&desc->context.unnamed_entry->context->context_desc);
        default: return NULL;
    }
}

static brooks_unnamed_entry_t *array_entry_create(brooks_pool_t *pool, brooks_array_t *context)
{
    brooks_unnamed_entry_t *entry = brooks_pool_malloc(pool, sizeof(brooks_unnamed_entry_t));
    entry->context = context;
    entry->idx = context->num_entries;
    return entry;
}

static brooks_element_t *element_create(brooks_pool_t *pool, brooks_entry_type_e entry_type, void *entry, size_t idx)
{
    brooks_element_t *element = brooks_pool_malloc(pool, sizeof(brooks_element_t));
    element->entry.context_type = entry_type;
    element->idx = idx;
    switch (entry_type) {
        case brooks_entry_type_named_entry:
            element->entry.context.named_entry = entry;
            break;
        case brooks_entry_type_unnamed_entry:
            element->entry.context.unnamed_entry = entry;
            break;
        default:
            return NULL;
    }
    return element;
}
