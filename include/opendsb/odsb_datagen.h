#ifndef BROOKS_MB_DATAGEN_H
#define BROOKS_MB_DATAGEN_H

#include <brooks/brooks_pool.h>
#include <brooks/brooks_doc.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum odsb_prop_type_e {
    prop_type_integer = 0,
    prop_type_boolean = 1,
    prop_type_decimal = 2,
    prop_type_string = 3,
    prop_type_array_integer = 4,
    prop_type_array_boolean = 5,
    prpo_type_array_decimal = 6,
    prop_type_array_string = 7,
    prop_type_array_object = 8,
    prop_type_array_array = 9,
    prop_type_object = 10
} odsb_prop_type_e;

/**
 * Describes the properties histogram for creating objects.
 */
typedef struct odsb_prop_gen_params_t
{
    unsigned hist_num_integer;
    unsigned hist_num_boolean;
    unsigned hist_num_decimal;
    unsigned hist_num_string;

    unsigned hist_num_array_integer;
    unsigned hist_num_array_boolean;
    unsigned hist_num_array_decimal;
    unsigned hist_num_array_string;
    unsigned hist_num_array_object;
    unsigned hist_num_array_array;

    unsigned hist_num_object;

} odsb_prop_gen_params_t;

typedef struct odsb_table_t
{
    void *base;
    size_t elem_size;
    size_t num_entries;
    size_t capacity;
} odsb_table_t;

// Random number in right-open interval [0, max)
static inline size_t odsb_random(long max) {
    size_t num;

    size_t bins = (size_t) max;
    size_t rand = (size_t) RAND_MAX + 1;
    size_t size = rand / bins;
    size_t outline   = rand % bins;

    do {
        num = random();
    } while (rand - outline <= (size_t)num);

    return num/size;
}

static inline odsb_table_t *odbs_table_new(size_t num_elements, size_t elem_size)
{
    odsb_table_t *result = malloc(sizeof(odsb_table_t));
    result->base = malloc(num_elements * elem_size);
    result->capacity = num_elements;
    result->num_entries = 0;
    result->elem_size = elem_size;
    return result;
}

static inline const void *odbs_table_random(odsb_table_t *table)
{
    return (table->base + odsb_random(table->num_entries) * table->elem_size);
}

static inline void odbs_table_add(odsb_table_t *table, const void *data, size_t multiplier)
{
    for (size_t i = 0; i < multiplier; i++) {
        if (table->num_entries < table->capacity) {
            memcpy(table->base + table->num_entries * table->elem_size, data, table->elem_size);
            table->num_entries++;
        } else {
            abort();
        }
    }
}

static inline odsb_table_t *odbs_table_by_prop(const odsb_prop_gen_params_t *params)
{
    size_t total = params->hist_num_integer +
            params->hist_num_boolean +
            params->hist_num_decimal +
            params->hist_num_string +
            params->hist_num_array_integer +
            params->hist_num_array_boolean +
            params->hist_num_array_decimal +
            params->hist_num_array_string +
            params->hist_num_array_object +
            params->hist_num_array_array +
            params->hist_num_object;

    odsb_table_t *result = odbs_table_new(total, sizeof(odsb_prop_type_e));

    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_integer}, params->hist_num_integer);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_boolean}, params->hist_num_boolean);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_decimal}, params->hist_num_decimal);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_string}, params->hist_num_string);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_array_integer}, params->hist_num_array_integer);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_array_boolean}, params->hist_num_array_boolean);
    odbs_table_add(result, &(odsb_prop_type_e) {prpo_type_array_decimal}, params->hist_num_array_decimal);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_array_string}, params->hist_num_array_string);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_array_object}, params->hist_num_array_object);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_array_array}, params->hist_num_array_array);
    odbs_table_add(result, &(odsb_prop_type_e) {prop_type_object}, params->hist_num_object);

    return result;
}

typedef struct odsb_histogram_t
{
    void **data_list;
    unsigned num_elements_data_list;
    unsigned capacity_data_list;
    unsigned *histogram;
    unsigned hist_capacity;
    unsigned num_hist_entries;
} odsb_histogram_t;

static inline odsb_histogram_t *odsb_histogram_new(size_t key_capacity)
{
    odsb_histogram_t *result = malloc(sizeof(odsb_histogram_t));
    result->data_list = malloc(key_capacity * sizeof(void *));
    result->histogram = malloc(key_capacity * sizeof(unsigned));
    result->num_elements_data_list = result->num_hist_entries = 0;
    result->capacity_data_list = result->hist_capacity = key_capacity;
    return result;
}

static inline void odsb_generic_histogram_add(odsb_histogram_t *hist, const void *data, size_t data_sizeof, size_t num_data, unsigned nums)
{
    if (hist->num_elements_data_list + 1 > hist->capacity_data_list) {
        hist->capacity_data_list = (hist->capacity_data_list) * 2;
        hist->data_list = realloc(hist->data_list, hist->capacity_data_list * sizeof(char *));
    }
    if (hist->num_hist_entries + nums > hist->hist_capacity) {
        hist->hist_capacity = (hist->hist_capacity + nums + 1) * 2;
        hist->histogram = realloc(hist->histogram, hist->hist_capacity * sizeof(unsigned));
    }
    void *data_entry = malloc(num_data * data_sizeof);
    memcpy(data_entry, data, num_data * data_sizeof);
    hist->data_list[hist->num_elements_data_list] = data_entry;
    while (nums--) {
        hist->histogram[hist->num_hist_entries++] = hist->num_elements_data_list;
    }
    hist->num_elements_data_list++;
}

static inline void odsb_histogram_key_add(odsb_histogram_t *hist, const char *key, unsigned nums)
{
    odsb_generic_histogram_add(hist, key, sizeof(char), strlen(key) + 1, nums);
}

static inline void odsb_histogram_value_int_add(odsb_histogram_t *hist, uint64_t number, unsigned nums)
{
    odsb_generic_histogram_add(hist, &number, sizeof(uint64_t), 1, nums);
}

static inline void odsb_histogram_value_double_add(odsb_histogram_t *hist, double number, unsigned nums)
{
    odsb_generic_histogram_add(hist, &number, sizeof(double), 1, nums);
}

static inline void odsb_histogram_value_boolean_add(odsb_histogram_t *hist, bool value, unsigned nums)
{
    odsb_generic_histogram_add(hist, &value, sizeof(bool), 1, nums);
}

static inline void odsb_histogram_value_string_add(odsb_histogram_t *hist, const char *value, unsigned nums)
{
    odsb_generic_histogram_add(hist, value, sizeof(char), strlen(value) + 1, nums);
}

static inline void odsb_histogram_value_array_add(odsb_histogram_t *hist, const brooks_array_t *value, unsigned nums)
{
    odsb_generic_histogram_add(hist, &value, sizeof(brooks_array_t*), 1, nums);
}

static inline void odsb_histogram_value_object_add(odsb_histogram_t *hist, const brooks_object_t *value, unsigned nums)
{
    odsb_generic_histogram_add(hist, &value, sizeof(brooks_object_t*), 1, nums);
}

static inline const void *odsb_histogram_random(const odsb_histogram_t *hist)
{
    return hist->data_list[hist->histogram[odsb_random(hist->num_hist_entries)]];
}

typedef struct odsb_key_gen_params_t
{
    odsb_histogram_t *val_int;
    odsb_histogram_t *val_boolean;
    odsb_histogram_t *val_decimal;
    odsb_histogram_t *val_string;

    odsb_histogram_t *array_int;
    odsb_histogram_t *array_boolean;
    odsb_histogram_t *array_decimal;
    odsb_histogram_t *array_string;
    odsb_histogram_t *array_array;
    odsb_histogram_t *array_object;

    odsb_histogram_t *object;
} odsb_key_gen_params_t;

static inline odsb_key_gen_params_t *odsb_key_gen_new(odsb_histogram_t *val_int,
                                                      odsb_histogram_t *val_boolean,
                                                      odsb_histogram_t *val_decimal,
                                                      odsb_histogram_t *val_string,
                                                      odsb_histogram_t *array_int,
                                                      odsb_histogram_t *array_boolean,
                                                      odsb_histogram_t *array_decimal,
                                                      odsb_histogram_t *array_string,
                                                      odsb_histogram_t *array_array,
                                                      odsb_histogram_t *array_object,
                                                      odsb_histogram_t *object)
{
    odsb_key_gen_params_t *result = malloc(sizeof(odsb_key_gen_params_t));
    result->val_int = val_int;
    result->val_boolean = val_boolean;
    result->val_decimal = val_decimal;
    result->val_string = val_string;
    result->array_int = array_int;
    result->array_boolean = array_boolean;
    result->array_decimal = array_decimal;
    result->array_string = array_string;
    result->array_array = array_array;
    result->array_object = array_object;
    result->object = object;
    return result;
}

typedef struct odsb_value_gen_params_t
{
    odsb_histogram_t *val_int;
    odsb_histogram_t *val_boolean;
    odsb_histogram_t *val_decimal;
    odsb_histogram_t *val_string;

    odsb_histogram_t *array_int;
    odsb_histogram_t *array_boolean;
    odsb_histogram_t *array_decimal;
    odsb_histogram_t *array_string;
    odsb_histogram_t *array_array;
    odsb_histogram_t *array_object;

    odsb_histogram_t *object;
} odsb_value_gen_params_t;

static inline odsb_value_gen_params_t *odsb_value_gen_new(odsb_histogram_t *val_int,
                                                      odsb_histogram_t *val_boolean,
                                                      odsb_histogram_t *val_decimal,
                                                      odsb_histogram_t *val_string,
                                                      odsb_histogram_t *array_int,
                                                      odsb_histogram_t *array_boolean,
                                                      odsb_histogram_t *array_decimal,
                                                      odsb_histogram_t *array_string,
                                                      odsb_histogram_t *array_array,
                                                      odsb_histogram_t *array_object,
                                                      odsb_histogram_t *object)
{
    odsb_value_gen_params_t *result = malloc(sizeof(odsb_value_gen_params_t));
    result->val_int = val_int;
    result->val_boolean = val_boolean;
    result->val_decimal = val_decimal;
    result->val_string = val_string;
    result->array_int = array_int;
    result->array_boolean = array_boolean;
    result->array_decimal = array_decimal;
    result->array_string = array_string;
    result->array_array = array_array;
    result->array_object = array_object;
    result->object = object;
    return result;
}

static inline const char *odsb_key_gen_random(odsb_key_gen_params_t *hist, odsb_prop_type_e type)
{
    switch (type) {
        case prop_type_integer:         return odsb_histogram_random(hist->val_int);
        case prop_type_boolean:        return odsb_histogram_random(hist->val_boolean);
        case prop_type_decimal:         return odsb_histogram_random(hist->val_decimal);
        case prop_type_string:          return odsb_histogram_random(hist->val_string);
        case prop_type_array_integer:   return odsb_histogram_random(hist->array_int);
        case prop_type_array_boolean:   return odsb_histogram_random(hist->array_boolean);
        case prpo_type_array_decimal:   return odsb_histogram_random(hist->array_decimal);
        case prop_type_array_string:    return odsb_histogram_random(hist->array_string);
        case prop_type_array_object:    return odsb_histogram_random(hist->array_object);
        case prop_type_array_array:     return odsb_histogram_random(hist->array_array);
        case prop_type_object:          return odsb_histogram_random(hist->object);
        default: abort();
    }
}

static inline const void *odsb_value_gen_random(odsb_key_gen_params_t *hist, odsb_prop_type_e type)
{
    switch (type) {
        case prop_type_integer:         return odsb_histogram_random(hist->val_int);
        case prop_type_boolean:         return odsb_histogram_random(hist->val_boolean);
        case prop_type_decimal:         return odsb_histogram_random(hist->val_decimal);
        case prop_type_string:          return odsb_histogram_random(hist->val_string);
        case prop_type_array_integer:   return odsb_histogram_random(hist->array_int);
        case prop_type_array_boolean:   return odsb_histogram_random(hist->array_boolean);
        case prpo_type_array_decimal:   return odsb_histogram_random(hist->array_decimal);
        case prop_type_array_string:    return odsb_histogram_random(hist->array_string);
        case prop_type_array_object:    return odsb_histogram_random(hist->array_object);
        case prop_type_array_array:     return odsb_histogram_random(hist->array_array);
        case prop_type_object:          return odsb_histogram_random(hist->object);
        default: abort();
    }
}








typedef struct odsb_value_histogram_t
{
    void **values;
    unsigned num_values;
    unsigned *histogram;
} odsb_value_histogram_t;

typedef struct odsb_values_gen_params_t
{
    odsb_value_histogram_t val_int;
    odsb_value_histogram_t val_boolean;
    odsb_value_histogram_t val_decimal;
    odsb_value_histogram_t val_string;

    odsb_value_histogram_t array_int;
    odsb_value_histogram_t array_boolean;
    odsb_value_histogram_t array_decimal;
    odsb_value_histogram_t array_string;
    odsb_value_histogram_t array_array;
    odsb_value_histogram_t array_object;

    odsb_value_histogram_t object_int;
    odsb_value_histogram_t object_boolean;
    odsb_value_histogram_t object_decimal;
    odsb_value_histogram_t object_string;
    odsb_value_histogram_t object_array;
    odsb_value_histogram_t object_object;
} odsb_values_gen_params_t;



static inline void odbs_document_make(brooks_object_t *document, brooks_pool_t *pool)
{
    brooks_doc_create(&document, pool);

}



static inline odsb_table_t *odbs_table_make_by_prop(const odsb_prop_gen_params_t *prop)
{
    odsb_table_t *result = malloc(sizeof(odsb_table_t *));
    size_t idx;
    size_t total = prop->hist_num_integer + prop->hist_num_boolean + prop->hist_num_decimal +
            prop->hist_num_string + prop->hist_num_array_integer + prop->hist_num_array_boolean +
            prop->hist_num_array_decimal + prop->hist_num_array_string +
            prop->hist_num_array_object + prop->hist_num_array_array +
            prop->hist_num_object;

    odsb_prop_type_e *tab_properties = malloc(total * sizeof(odsb_prop_type_e));
    idx = 0;

    for (size_t i = 0; i < prop->hist_num_integer; i++)
        tab_properties[idx++] = prop_type_integer;
    for (size_t i = 0; i < prop->hist_num_boolean; i++)
        tab_properties[idx++] = prop_type_boolean;
    for (size_t i = 0; i < prop->hist_num_decimal; i++)
        tab_properties[idx++] = prop_type_decimal;
    for (size_t i = 0; i < prop->hist_num_string; i++)
        tab_properties[idx++] = prop_type_string;
    for (size_t i = 0; i < prop->hist_num_array_integer; i++)
        tab_properties[idx++] = prop_type_array_integer;
    for (size_t i = 0; i < prop->hist_num_array_boolean; i++)
        tab_properties[idx++] = prop_type_array_boolean;
    for (size_t i = 0; i < prop->hist_num_array_decimal; i++)
        tab_properties[idx++] = prpo_type_array_decimal;
    for (size_t i = 0; i < prop->hist_num_array_string; i++)
        tab_properties[idx++] = prop_type_array_string;
    for (size_t i = 0; i < prop->hist_num_array_object; i++)
        tab_properties[idx++] = prop_type_array_object;
    for (size_t i = 0; i < prop->hist_num_array_array; i++)
        tab_properties[idx++] = prop_type_array_array;
    for (size_t i = 0; i < prop->hist_num_object; i++)
        tab_properties[idx++] = prop_type_object;

    result->num_entries = total;
    result->base = tab_properties;
    return result;
}


/**
 * Create an object having randomly generated properties (objects, arrays, primitives) according a given histogram.
 * Actually, it's generating a list of "something" of a certain type according a given non-uniform-random
 * distribution by this histogram. For ideas on how to generate numbers according a nun-uniform distribution, see
 * see https://oroboro.com/non-uniform-random-numbers/.
 */
static inline void odbs_object_make(brooks_object_t *parent, brooks_pool_t *pool,
                                    const odsb_prop_gen_params_t *prop_hist,
                                    const odsb_key_gen_params_t *key_gen_params,
                                    const odsb_values_gen_params_t *values_gen_params)
{



}

#endif //BROOKS_MB_DATAGEN_H
