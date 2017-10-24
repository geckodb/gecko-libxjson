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

#include <xjson/xjson-json.h>
#include <xjson/xjson-misc.h>

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef enum
{
    xjson_entry_type_named_entry, xjson_entry_type_unnamed_entry
} xjson_entry_type_e;

typedef struct xjson_entry_desc_t
{
    union {
        xjson_named_entry_t         *named_entry;
        xjson_unnamed_entry_t       *unnamed_entry;
    } context;
    xjson_entry_type_e               context_type;
} xjson_entry_desc_t;

typedef struct xjson_object_t
{
    xjson_entry_desc_t            context_desc;
    xjson_named_entry_t         **entries;
    xjson_u64_t                   num_entries;
    xjson_u64_t                   capacity;
    xjson_u64_t                   idx;
    xjson_pool_t                 *pool;
} xjson_object_t;

typedef struct xjson_array_t
{
    xjson_entry_desc_t          context_desc;
    xjson_u64_t                   num_entries;
    xjson_u64_t                   capacity;
    xjson_type_e                  type;
    xjson_unnamed_entry_t       **entries;
} xjson_array_t;

typedef struct xjson_unnamed_entry_t
{
    xjson_array_t                *context;
    xjson_u64_t                   idx;
    xjson_value_t                *value;

} xjson_unnamed_entry_t;

typedef struct xjson_named_entry_t
{
    xjson_object_t               *context;
    char                         *key;
    xjson_value_t                *value;
} xjson_named_entry_t;

typedef struct xjson_element_t {
    xjson_entry_desc_t            entry;
} xjson_element_t;

typedef struct xjson_value_t
{
    xjson_entry_desc_t            context_desc;
    xjson_type_e                  type;
    union {
        xjson_array_t            *array;
        xjson_object_t           *object;
        xjson_s64_t               integer;
        xjson_double_t            decimal;
        xjson_string_t            string;
        xjson_boolean_t           boolean;
    };
} xjson_value_t;

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static xjson_object_t *json_create(xjson_pool_t *pool, xjson_entry_type_e parent_type, void *parent_ptr);
static xjson_status_e json_autoresize(xjson_object_t *object);
static xjson_status_e array_autoresize(xjson_array_t *array);
static void json_add_entry(xjson_object_t *object, xjson_named_entry_t *entry);
static xjson_status_e json_add_complex(xjson_object_t **object, xjson_array_t **array, xjson_object_t *parent,
                                       const char *key, xjson_type_e complex_type, xjson_type_e array_type);
static xjson_value_t *value_create(xjson_pool_t *pool, xjson_type_e type, xjson_entry_type_e context,
                                   void *parent_ptr);
static xjson_named_entry_t *entry_create(xjson_object_t *object, xjson_type_e type, const char *key);
static xjson_array_t *array_create(xjson_type_e type, xjson_entry_type_e context, void *parent_ptr);
static xjson_pool_t *context_get_pool(xjson_entry_desc_t *desc);
static xjson_unnamed_entry_t *array_entry_create(xjson_pool_t *pool, xjson_array_t *context);
static xjson_element_t *element_create(xjson_pool_t *pool, xjson_entry_type_e entry_type, void *entry);

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

xjson_status_e xjson_json_create(xjson_object_t **json, xjson_pool_t *pool)
{
    *json = json_create(pool, xjson_entry_type_named_entry, XJSON_ROOT);
    return (*json != NULL ? xjson_status_ok : (pool != NULL ? xjson_status_failed : xjson_status_nopool));
}

xjson_status_e xjson_json_parse(xjson_object_t **json, const char *text)
{
    // TODO:...
    return xjson_status_interalerr;
}

xjson_status_e xjson_json_print(FILE *file, const xjson_object_t *json)
{
    if (file && json) {
        fprintf(file, "{ ");
        for (xjson_u64_t i = 0; i < json->num_entries; i++) {
            xjson_named_entry_t *entry = json->entries[i];
            printf("\"%s\": ", entry->key);
            switch (entry->value->type) {
                case xjson_object:
                    xjson_json_print(file, entry->value->object);
                    break;
                case xjson_array:
                    xjson_array_print(file, entry->value->array);
                    break;
                case xjson_number_integer:
                    fprintf(file, "%lld", entry->value->integer);
                    break;
                case xjson_number_double:
                    fprintf(file, "%f", entry->value->decimal);
                    break;
                case xjson_string:
                    fprintf(file, "\"%s\"", entry->value->string);
                    break;
                case xjson_boolean:
                    fprintf(file, "\"%s\"", (entry->value->boolean ? "true" : "false"));
                    break;
                case xjson_null:
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
    return xjson_status_ok;
}

xjson_status_e value_set_primitive(xjson_value_t *value, xjson_pool_t *pool, xjson_type_e type, const void *data)
{
    switch (type) {
        case xjson_number_integer:
            memcpy(&value->integer, data, sizeof(xjson_s64_t));
            break;
        case xjson_number_double:
            memcpy(&value->decimal, data, sizeof(xjson_double_t));
            break;
        case xjson_string:
            value->string = xjson_misc_strdup(pool, data);
            break;
        case xjson_boolean:
            memcpy(&value->boolean, data, sizeof(xjson_boolean_t));
            break;
        case xjson_null:
            break;
        case xjson_object:
        case xjson_array:
            return xjson_status_wrongusage;
        default:
            return xjson_status_notype;
    }
    return xjson_status_ok;
}

xjson_status_e xjson_json_add_property(xjson_object_t *parent, xjson_type_e type, const char *key, const void *data)
{
    if (parent != NULL && (type == xjson_null || data != NULL)) {
        xjson_named_entry_t *entry = entry_create(parent, type, key);
        if (entry != NULL && (json_autoresize(parent) == xjson_status_ok)) {
            xjson_status_e status = value_set_primitive(entry->value, parent->pool, type, data);
            if (status == xjson_status_ok) {
                json_add_entry(parent, entry);
                return xjson_status_ok;
            } else {
                return status;
            }
        } else return xjson_status_pmalloc_err;
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_json_add_object(xjson_object_t **object, xjson_object_t *parent, const char *key)
{
    return json_add_complex(object, NULL, parent, key, xjson_object, 0);
}

xjson_status_e xjson_json_add_array(xjson_array_t **array, xjson_object_t *parent, xjson_type_e type, const char *key)
{
    return json_add_complex(NULL, array, parent, key, xjson_array, type);
}

xjson_status_e xjson_array_add_value(xjson_array_t *parent, const void *data)
{
    xjson_status_e status;
    if (parent != NULL && data != NULL) {
        xjson_pool_t *pool = context_get_pool(&parent->context_desc);
        xjson_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = value_create(pool, parent->type, xjson_entry_type_unnamed_entry, parent);
        if (((status = value_set_primitive(entry->value, pool, parent->type, data)) == xjson_status_ok) &&
                (status = array_autoresize(parent)) == xjson_status_ok) {
            parent->entries[parent->num_entries++] = entry;
            return xjson_status_ok;
        } else {
            return status;
        }
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_array_add_object(xjson_object_t **object, xjson_array_t *parent)
{
    xjson_status_e status;
    if (parent != NULL) {
        xjson_pool_t *pool = context_get_pool(&parent->context_desc);
        status = array_autoresize(parent);
        xjson_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = xjson_pool_malloc(pool, sizeof(xjson_value_t));
        entry->value->object = json_create(pool, xjson_entry_type_unnamed_entry, entry);
        parent->entries[parent->num_entries++] = entry;
        *object = entry->value->object;
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_array_add_array(xjson_array_t **array, xjson_type_e type, xjson_array_t *parent)
{
    xjson_status_e status;
    if (parent != NULL) {
        xjson_pool_t *pool = context_get_pool(&parent->context_desc);
        status = array_autoresize(parent);
        xjson_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = xjson_pool_malloc(pool, sizeof(xjson_value_t));
        entry->value->array = array_create(type, xjson_entry_type_unnamed_entry, entry);
        parent->entries[parent->num_entries++] = entry;
        *array = entry->value->array;
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_array_print(FILE *file, const xjson_array_t *array)
{
    if (file && array) {
        printf("[ ");
        for (xjson_u64_t i = 0; i < array->num_entries; i++) {
            xjson_unnamed_entry_t *entry = array->entries[i];
            switch (array->type) {
                case xjson_object:
                    xjson_json_print(file, entry->value->object);
                    break;
                case xjson_array:
                    xjson_array_print(file, entry->value->array);
                    break;
                case xjson_number_integer:
                    fprintf(file, "%lld", entry->value->integer);
                    break;
                case xjson_number_double:
                    fprintf(file, "%f", entry->value->decimal);
                    break;
                case xjson_string:
                    fprintf(file, "\"%s\"", entry->value->string);
                    break;
                case xjson_boolean:
                    fprintf(file, "\"%s\"", (entry->value->boolean ? "true" : "false"));
                    break;
                case xjson_null:
                    fprintf(file, "null");
                    break;
                default:
                    fprintf(file, "(unknown)");
                    break;
            }
            fprintf(file, "%s ", (i + 1 < array->num_entries ? "," : ""));
        }
        printf("]");
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_scan(xjson_element_t **element, const xjson_object_t *object)
{
    static const xjson_object_t *source;
    static xjson_u64_t idx;
    if (object != NULL) {
        source = object;
        idx = 0;
    }
    if (idx < source->num_entries) {
        xjson_named_entry_t *entry = source->entries[idx];
        xjson_element_t *result = element_create(source->pool, xjson_entry_type_named_entry, entry);
        *element = result;
        idx++;
        return xjson_status_ok;
    } else return xjson_status_eof;
}

xjson_status_e xjson_element_has_key(xjson_element_t *element)
{
    return ((element != NULL && element->entry.context_type == xjson_entry_type_named_entry) ?
            xjson_status_true : xjson_status_false);
}

xjson_status_e xjson_element_get(const char **key, const xjson_value_t **value, xjson_element_t *element)
{
    if (element) {
        if (key && xjson_element_has_key(element)) {
            *key = element->entry.context.named_entry->key;
        } else {
            return xjson_status_false;
        }
        if (value) {
            switch (element->entry.context_type) {
                case xjson_entry_type_named_entry:
                    *value = element->entry.context.named_entry->value;
                    break;
                case xjson_entry_type_unnamed_entry:
                    *value = element->entry.context.unnamed_entry->value;
                    break;
                default:
                    return xjson_status_interalerr;
            }
        }
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_value_get_type(xjson_type_e *type, const xjson_value_t *value)
{
    if (type && value) {
        *type = value->type;
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

const char *xjson_type_str(const xjson_type_e type)
{
    switch (type) {
        case xjson_none:            return "none";
        case xjson_object:          return "object";
        case xjson_array:           return "array";
        case xjson_number_integer:  return "number_u64";
        case xjson_number_double:   return "number_double";
        case xjson_string:          return "string";
        case xjson_boolean:         return "boolean";
        case xjson_null:            return "null";
        default: return NULL;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static xjson_object_t *json_create(xjson_pool_t *pool, xjson_entry_type_e parent_type, void *parent_ptr)
{
    xjson_object_t *retval = NULL;
    if ((pool != NULL) &&
        ((retval = xjson_pool_malloc(pool, sizeof(xjson_object_t))) != NULL) &&
        ((retval->entries = xjson_pool_malloc(pool, XJSON_JSON_ENTRIES_CAPACITY * sizeof(xjson_named_entry_t *))) != NULL)) {
        retval->capacity = XJSON_JSON_ENTRIES_CAPACITY;
        retval->idx = retval->num_entries = 0;
        retval->context_desc.context_type = parent_type;
        switch (parent_type) {
            case xjson_entry_type_named_entry:
                retval->context_desc.context.named_entry = parent_ptr;
                break;
            case xjson_entry_type_unnamed_entry:
                retval->context_desc.context.unnamed_entry = parent_ptr;
                break;
            default: return NULL;
        }
        retval->pool = pool;
    }
    return retval;
}

static xjson_status_e json_autoresize(xjson_object_t *object)
{
    object->entries = xjson_misc_autoresize(object->entries, sizeof(xjson_named_entry_t *), &object->num_entries,
                                         &object->capacity);
    return (object->entries != NULL ? xjson_status_ok : xjson_status_pmalloc_err);
}

static xjson_status_e array_autoresize(xjson_array_t *array)
{
    array->entries = xjson_misc_autoresize(array->entries, sizeof(xjson_unnamed_entry_t *), &array->num_entries,
                                        &array->capacity);
    return (array->entries != NULL ? xjson_status_ok : xjson_status_pmalloc_err);
}

static void json_add_entry(xjson_object_t *object, xjson_named_entry_t *entry)
{
    object->entries[object->num_entries++] = entry;
}

static xjson_status_e json_add_complex(xjson_object_t **object, xjson_array_t **array, xjson_object_t *parent,
                                       const char *key, xjson_type_e complex_type, xjson_type_e array_type)
{
    xjson_object_t *retval_object;
    xjson_array_t *retval_array;
    xjson_named_entry_t *entry;
    xjson_status_e status;

    if (complex_type != xjson_array && complex_type != xjson_object) {
        return xjson_status_interalerr;
    } else {
        if (parent != NULL && key != NULL && ((entry = entry_create(parent, complex_type, key)) != NULL) &&
            (((complex_type != xjson_object) ||
                    ((retval_object = json_create(parent->pool, xjson_entry_type_named_entry, entry)) != NULL)) &&
             ((complex_type != xjson_array) ||
                     ((retval_array = array_create(array_type, xjson_entry_type_named_entry, entry)) != NULL)))) {
            if ((status = json_autoresize(parent)) == xjson_status_ok) {
                json_add_entry(parent, entry);
                if (complex_type == xjson_object) {
                    entry->value->object = retval_object;
                    *object = retval_object;
                } else {
                    entry->value->array = retval_array;
                    *array = retval_array;
                }
                return xjson_status_ok;
            } return status;
        } else return xjson_status_nullptr;
    }
}

static xjson_value_t *value_create(xjson_pool_t *pool, xjson_type_e type, xjson_entry_type_e context,
                                   void *parent_ptr)
{
    xjson_value_t *value = xjson_pool_malloc(pool, sizeof(xjson_value_t));
    value->type = type;
    value->context_desc.context_type = context;
    switch (context) {
        case xjson_entry_type_named_entry:
            value->context_desc.context.named_entry = parent_ptr;
            break;
        case xjson_entry_type_unnamed_entry:
            value->context_desc.context.unnamed_entry = parent_ptr;
            break;
        default: return NULL;
    }
    return value;
}

static xjson_named_entry_t *entry_create(xjson_object_t *object, xjson_type_e type, const char *key)
{
    xjson_named_entry_t *entry = xjson_pool_malloc(object->pool, sizeof(xjson_named_entry_t));
    xjson_value_t *value = value_create(object->pool, type, xjson_entry_type_named_entry, entry);
    entry->context = object;
    entry->key = xjson_misc_strdup(object->pool, key);
    entry->value = value;
    return entry;
}

static xjson_array_t *array_create(xjson_type_e type, xjson_entry_type_e context, void *parent_ptr)
{
    xjson_entry_desc_t context_desc;
    switch (context) {
        case xjson_entry_type_unnamed_entry:
            context_desc.context.unnamed_entry = parent_ptr;
            break;
        case xjson_entry_type_named_entry:
            context_desc.context.named_entry = parent_ptr;
            break;
        default: return NULL;
    }
    context_desc.context_type = context;
    xjson_pool_t *pool = context_get_pool(&context_desc);
    xjson_array_t *retval = xjson_pool_malloc(pool, sizeof(xjson_array_t));
    retval->context_desc = context_desc;
    retval->num_entries = 0;
    retval->type = type;
    retval->capacity = XJSON_ARRAY_CAPACITY;
    retval->entries = xjson_pool_malloc(pool, XJSON_ARRAY_CAPACITY * sizeof(xjson_unnamed_entry_t *));
    return retval;
}

static xjson_pool_t *context_get_pool(xjson_entry_desc_t *desc)
{
    switch (desc->context_type) {
        case xjson_entry_type_named_entry:
            return desc->context.named_entry->context->pool;
        case xjson_entry_type_unnamed_entry:
            return context_get_pool(&desc->context.unnamed_entry->context->context_desc);
        default: return NULL;
    }
}

static xjson_unnamed_entry_t *array_entry_create(xjson_pool_t *pool, xjson_array_t *context)
{
    xjson_unnamed_entry_t *entry = xjson_pool_malloc(pool, sizeof(xjson_unnamed_entry_t));
    entry->context = context;
    entry->idx = context->num_entries;
    return entry;
}

static xjson_element_t *element_create(xjson_pool_t *pool, xjson_entry_type_e entry_type, void *entry)
{
    xjson_element_t *element = xjson_pool_malloc(pool, sizeof(xjson_element_t));
    element->entry.context_type = entry_type;
    switch (entry_type) {
        case xjson_entry_type_named_entry:
            element->entry.context.named_entry = entry;
            break;
        case xjson_entry_type_unnamed_entry:
            element->entry.context.unnamed_entry = entry;
            break;
        default:
            return NULL;
    }
    return element;
}
