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

// ---------------------------------------------------------------------------------------------------------------------
// M A C R O S
// ---------------------------------------------------------------------------------------------------------------------

#define MAKE_PTR_VECTOR(type, name)                                                                                    \
    type **name;                                                                                                       \
    xjson_u64_t name##_num_elements;                                                                                   \
    xjson_u64_t name##_capacity;

#define INIT_PTR_VECTOR(pool, type, name, capacity)                                                                    \
    pool->name##_capacity = capacity;                                                                                  \
    pool->name = malloc(retval->name##_capacity * sizeof(type *));                                                     \
    pool->name##_num_elements = 0;

#define FREE_PTR_VECTOR(pool, type, name)                                                                              \
    {                                                                                                                  \
        type **it = pool->name;                                                                                        \
        while (pool->name##_num_elements--) {                                                                          \
            free(*it);                                                                                                 \
            it++;                                                                                                      \
        }                                                                                                              \
        free(pool->name);                                                                                              \
    }

#define POOL_MALLOC_GENERIC(pool, section_name, type, multiplier)                                                      \
({                                                                                                                     \
    type *retval = malloc(multiplier * sizeof(type));                                                                  \
    pool->section_name = generic_autoresize(pool->section_name, sizeof(type *), &pool->section_name##_num_elements, &pool->section_name##_capacity); \
    pool->section_name[pool->section_name##_num_elements++] = retval;                                                  \
    retval;                                                                                                            \
})

#define POOL_MALLOC_OBJECT(pool)                                                                                       \
    POOL_MALLOC_GENERIC(pool, objects, xjson_json_t, 1)

#define POOL_MALLOC_ARRAY(pool)                                                                                        \
    POOL_MALLOC_GENERIC(pool, arrays, xjson_array_t, 1)

#define POOL_MALLOC_ENTRY(pool)                                                                                        \
    POOL_MALLOC_GENERIC(pool, entries, xjson_named_entry_t, 1)

#define POOL_MALLOC_VALUE(pool)                                                                                        \
    POOL_MALLOC_GENERIC(pool, values, xjson_value_t, 1)

#define POOL_MALLOC_ARRAY_ENTRY(pool)                                                                                  \
    POOL_MALLOC_GENERIC(pool, array_entries, xjson_unnamed_entry_t, 1)

#define POOL_MALLOC_OBJECT(pool)                                                                                       \
    POOL_MALLOC_GENERIC(pool, objects, xjson_json_t, 1)

#define POOL_MALLOC_NAMED_ENTRY_VECTOR(pool, capacity)                                                                 \
    POOL_MALLOC_GENERIC(pool, named_entry_vectors, xjson_named_entry_t *, capacity)

#define POOL_MALLOC_UNNAMED_ENTRY_VECTOR(pool, capacity)                                                               \
    POOL_MALLOC_GENERIC(pool, unnamed_entry_vectors, xjson_unnamed_entry_t *, capacity)

#define POOL_STRDUP(pool, str)                                                                                         \
({                                                                                                                     \
    char *cpy = POOL_MALLOC_GENERIC(pool, strings, char, strlen(str) + 1);                                             \
    strcpy(cpy, str);                                                                                                  \
    cpy;                                                                                                               \
})

// ---------------------------------------------------------------------------------------------------------------------
// T Y P E S
// ---------------------------------------------------------------------------------------------------------------------

typedef enum
{
    xjson_context_named_entry, xjson_context_unnamed_entry
} xjson_context_e;

typedef struct xjson_pool_t
{
    MAKE_PTR_VECTOR(xjson_json_t, objects);
    MAKE_PTR_VECTOR(xjson_array_t, arrays);
    MAKE_PTR_VECTOR(xjson_unnamed_entry_t, array_entries);
    MAKE_PTR_VECTOR(xjson_named_entry_t, entries);
    MAKE_PTR_VECTOR(char, strings);
    MAKE_PTR_VECTOR(xjson_value_t, values);
    MAKE_PTR_VECTOR(xjson_named_entry_t *, named_entry_vectors);
    MAKE_PTR_VECTOR(xjson_unnamed_entry_t *, unnamed_entry_vectors);
} xjson_pool_t;

typedef struct xjson_context_desc_t
{
    union {
        xjson_named_entry_t         *named_entry;
        xjson_unnamed_entry_t       *unnamed_entry;
    } context;
    xjson_context_e          context_type;
} xjson_context_desc_t;

typedef struct xjson_json_t
{
    xjson_context_desc_t          context_desc;
    xjson_named_entry_t         **entries;
    xjson_u64_t                   num_entries;
    xjson_u64_t                   capacity;
    xjson_u64_t                   idx;
    xjson_pool_t                 *pool;
} xjson_json_t;

typedef struct xjson_array_t
{
    xjson_context_desc_t          context_desc;
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
    xjson_json_t                 *context;
    char                         *key;
    xjson_value_t                *value;
} xjson_named_entry_t;

typedef struct xjson_value_t
{
    xjson_context_desc_t          context_desc;
    xjson_type_e                  type;
    union {
        xjson_array_t            *array;
        xjson_json_t             *object;
        xjson_s64_t               integer;
        xjson_double_t            decimal;
        xjson_string_t            string;
        xjson_boolean_t           boolean;
    };
} xjson_value_t;

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   D E C L A R A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static void *generic_autoresize(void *base, xjson_u64_t elem_size, xjson_u64_t *num_entries,
                                xjson_u64_t *capacity);
static xjson_json_t *json_create(xjson_pool_t *pool, xjson_context_e parent_type, void *parent_ptr);
static xjson_status_e json_autoresize(xjson_json_t *object);
static xjson_status_e array_autoresize(xjson_array_t *array);
static void json_add_entry(xjson_json_t *object, xjson_named_entry_t *entry);
static xjson_status_e json_add_complex(xjson_json_t **object, xjson_array_t **array, xjson_json_t *parent,
                                       const char *key, xjson_type_e complex_type, xjson_type_e array_type);
static xjson_value_t *value_create(xjson_pool_t *pool, xjson_type_e type, xjson_context_e context,
                                   void *parent_ptr);
static xjson_named_entry_t *entry_create(xjson_json_t *object, xjson_type_e type, const char *key);
static xjson_array_t *array_create(xjson_type_e type, xjson_context_e context, void *parent_ptr);
static xjson_pool_t *context_get_pool(xjson_context_desc_t *desc);
static xjson_unnamed_entry_t *array_entry_create(xjson_pool_t *pool, xjson_array_t *context);

// ---------------------------------------------------------------------------------------------------------------------
// I N T E R F A C E   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

xjson_status_e xjson_pool_create(xjson_pool_t **pool)
{
    xjson_pool_t *retval;
    if ((retval = malloc(sizeof(xjson_pool_t))) != NULL) {
        INIT_PTR_VECTOR(retval, xjson_json_t, objects, XJSON_POOL_OBJS_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_array_t, arrays, XJSON_POOL_ARRAYS_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_unnamed_entry_t, array_entries, XJSON_POOL_UNNAMED_ENTRIES_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_named_entry_t, entries, XJSON_POOL_NAMED_ENTRIES_CAPACITY);
        INIT_PTR_VECTOR(retval, char *, strings, XJSON_POOL_STRINGS_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_value_t, values, XJSON_POOL_VALUES_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_named_entry_t *, named_entry_vectors, XJSON_POOL_NAMED_ENTRY_VECTORS_CAPACITY);
        INIT_PTR_VECTOR(retval, xjson_unnamed_entry_t *, unnamed_entry_vectors, XJSON_POOL_UNNAMED_ENTRY_VECTORS_CAPACITY);
        *pool = retval;
        return ((retval->arrays != NULL && retval->objects) ? xjson_status_ok : xjson_status_malloc_err);
    }
    return xjson_status_malloc_err;
}

xjson_status_e xjson_pool_dispose(xjson_pool_t *pool)
{
    FREE_PTR_VECTOR(pool, xjson_json_t, objects);
    FREE_PTR_VECTOR(pool, xjson_array_t, arrays);
    FREE_PTR_VECTOR(pool, xjson_unnamed_entry_t, array_entries);
    FREE_PTR_VECTOR(pool, xjson_named_entry_t, entries);
    FREE_PTR_VECTOR(pool, char, strings);
    FREE_PTR_VECTOR(pool, xjson_value_t, values);
    FREE_PTR_VECTOR(pool, xjson_named_entry_t *, named_entry_vectors);
    FREE_PTR_VECTOR(pool, xjson_unnamed_entry_t *, unnamed_entry_vectors);
    free(pool);
    return xjson_status_malloc_err;
}

xjson_status_e xjson_json_create(xjson_json_t **json, xjson_pool_t *pool)
{
    *json = json_create(pool, xjson_context_named_entry, XJSON_ROOT);
    return (*json != NULL ? xjson_status_ok : (pool != NULL ? xjson_status_failed : xjson_status_nopool));
}

xjson_status_e xjson_json_parse(xjson_json_t **json, const char *text)
{
    // TODO:...
    return xjson_status_interalerr;
}

xjson_status_e xjson_json_print(FILE *file, const xjson_json_t *json)
{
    // TODO:...
    return xjson_status_interalerr;
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
            value->string = POOL_STRDUP(pool, data);
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

xjson_status_e xjson_json_add_property(xjson_json_t *parent, xjson_type_e type, const char *key, const void *data)
{
    if (parent != NULL && data != NULL) {
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

xjson_status_e xjson_json_add_object(xjson_json_t **object, xjson_json_t *parent, const char *key)
{
    return json_add_complex(object, NULL, parent, key, xjson_object, 0);
}

xjson_status_e xjson_json_add_array(xjson_array_t **array, xjson_json_t *parent, xjson_type_e type, const char *key)
{
    return json_add_complex(NULL, array, parent, key, xjson_array, type);
}

xjson_status_e xjson_array_add_value(xjson_array_t *parent, const void *data)
{
    xjson_status_e status;
    if (parent != NULL && data != NULL) {
        xjson_pool_t *pool = context_get_pool(&parent->context_desc);
        xjson_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = value_create(pool, parent->type, xjson_context_unnamed_entry, parent);
        if (((status = value_set_primitive(entry->value, pool, parent->type, data)) == xjson_status_ok) &&
                (status = array_autoresize(parent)) == xjson_status_ok) {
            parent->entries[parent->num_entries++] = entry;
            return xjson_status_ok;
        } else {
            return status;
        }
    } else return xjson_status_nullptr;
}

xjson_status_e xjson_array_add_object(xjson_json_t **object, xjson_array_t *parent)
{
    xjson_status_e status;
    if (parent != NULL) {
        xjson_pool_t *pool = context_get_pool(&parent->context_desc);
        status = array_autoresize(parent);
        xjson_unnamed_entry_t *entry = array_entry_create(pool, parent);
        entry->value = POOL_MALLOC_VALUE(pool);
        entry->value->object = json_create(pool, xjson_context_unnamed_entry, entry);
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
        entry->value = POOL_MALLOC_VALUE(pool);
        entry->value->array = array_create(type, xjson_context_unnamed_entry, entry);
        parent->entries[parent->num_entries++] = entry;
        *array = entry->value->array;
        return xjson_status_ok;
    } else return xjson_status_nullptr;
}

// ---------------------------------------------------------------------------------------------------------------------
// H E L P E R   I M P L E M E N T A T I O N
// ---------------------------------------------------------------------------------------------------------------------

static xjson_json_t *json_create(xjson_pool_t *pool, xjson_context_e parent_type, void *parent_ptr)
{
    xjson_json_t *retval = NULL;
    if ((pool != NULL) &&
        ((retval = POOL_MALLOC_OBJECT(pool)) != NULL) &&
        ((retval->entries = POOL_MALLOC_NAMED_ENTRY_VECTOR(pool, XJSON_JSON_ENTRIES_CAPACITY)) != NULL)) {
        retval->capacity = XJSON_JSON_ENTRIES_CAPACITY;
        retval->idx = retval->num_entries = 0;
        retval->context_desc.context_type = parent_type;
        switch (parent_type) {
            case xjson_context_named_entry:
                retval->context_desc.context.named_entry = parent_ptr;
                break;
            case xjson_context_unnamed_entry:
                retval->context_desc.context.unnamed_entry = parent_ptr;
                break;
            default: return NULL;
        }
        retval->pool = pool;
    }
    return retval;
}

static void *generic_autoresize(void *base, xjson_u64_t elem_size, xjson_u64_t *num_entries,
                                xjson_u64_t *capacity)
{
    xjson_u64_t new_num_entires = *num_entries + 1;
    while (new_num_entires >= *capacity) {
        *capacity = (*capacity + 1) * 1.7f;
        return realloc(base, *capacity * elem_size);
    }
    return base;
}

static xjson_status_e json_autoresize(xjson_json_t *object)
{
    object->entries = generic_autoresize(object->entries, sizeof(xjson_named_entry_t *), &object->num_entries,
                                         &object->capacity);
    return (object->entries != NULL ? xjson_status_ok : xjson_status_pmalloc_err);
}

static xjson_status_e array_autoresize(xjson_array_t *array)
{
    array->entries = generic_autoresize(array->entries, sizeof(xjson_unnamed_entry_t *), &array->num_entries,
                                        &array->capacity);
    return (array->entries != NULL ? xjson_status_ok : xjson_status_pmalloc_err);
}

static void json_add_entry(xjson_json_t *object, xjson_named_entry_t *entry)
{
    object->entries[object->num_entries++] = entry;
}

static xjson_status_e json_add_complex(xjson_json_t **object, xjson_array_t **array, xjson_json_t *parent,
                                       const char *key, xjson_type_e complex_type, xjson_type_e array_type)
{
    xjson_json_t *retval_object;
    xjson_array_t *retval_array;
    xjson_named_entry_t *entry;
    xjson_status_e status;

    if (complex_type != xjson_array && complex_type != xjson_object) {
        return xjson_status_interalerr;
    } else {
        if (parent != NULL && key != NULL && ((entry = entry_create(parent, xjson_object, key)) != NULL) &&
            (((complex_type != xjson_object) ||
                    ((retval_object = json_create(parent->pool, xjson_context_named_entry, entry)) != NULL)) &&
             ((complex_type != xjson_array) ||
                     ((retval_array = array_create(array_type, xjson_context_named_entry, entry)) != NULL)))) {
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

static xjson_value_t *value_create(xjson_pool_t *pool, xjson_type_e type, xjson_context_e context,
                                   void *parent_ptr)
{
    xjson_value_t *value = POOL_MALLOC_VALUE(pool);
    value->type = type;
    value->context_desc.context_type = context;
    switch (context) {
        case xjson_context_named_entry:
            value->context_desc.context.named_entry = parent_ptr;
            break;
        case xjson_context_unnamed_entry:
            value->context_desc.context.unnamed_entry = parent_ptr;
            break;
        default: return NULL;
    }
    return value;
}

static xjson_named_entry_t *entry_create(xjson_json_t *object, xjson_type_e type, const char *key)
{
    xjson_named_entry_t *entry = POOL_MALLOC_ENTRY(object->pool);
    xjson_value_t *value = value_create(object->pool, type, xjson_context_named_entry, entry);
    entry->context = object;
    entry->key = POOL_STRDUP(object->pool, key);
    entry->value = value;
    return entry;
}

static xjson_array_t *array_create(xjson_type_e type, xjson_context_e context, void *parent_ptr)
{
    xjson_context_desc_t context_desc;
    switch (context) {
        case xjson_context_unnamed_entry:
            context_desc.context.unnamed_entry = parent_ptr;
            break;
        case xjson_context_named_entry:
            context_desc.context.named_entry = parent_ptr;
            break;
        default: return NULL;
    }
    context_desc.context_type = context;
    xjson_array_t *retval = POOL_MALLOC_ARRAY(context_get_pool(&context_desc));
    retval->context_desc = context_desc;
    retval->num_entries = 0;
    retval->type = type;
    retval->capacity = XJSON_ARRAY_CAPACITY;
    retval->entries = POOL_MALLOC_UNNAMED_ENTRY_VECTOR(context_get_pool(&context_desc), XJSON_ARRAY_CAPACITY);
    return retval;
}

static xjson_pool_t *context_get_pool(xjson_context_desc_t *desc)
{
    switch (desc->context_type) {
        case xjson_context_named_entry:
            return desc->context.named_entry->context->pool;
        case xjson_context_unnamed_entry:
            return context_get_pool(&desc->context.unnamed_entry->context->context_desc);
        default: return NULL;
    }
}

static xjson_unnamed_entry_t *array_entry_create(xjson_pool_t *pool, xjson_array_t *context)
{
    xjson_unnamed_entry_t *entry = POOL_MALLOC_ARRAY_ENTRY(pool);
    entry->context = context;
    entry->idx = context->num_entries;
    return entry;
}
