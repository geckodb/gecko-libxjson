
#include <opendsb/odsb_datagen.h>
#include <time.h>

int main(void) {

    odsb_prop_gen_params_t props = {
        .hist_num_integer    = 10,
        .hist_num_boolean    = 10,
        .hist_num_decimal    = 10,
        .hist_num_string     = 10,

        .hist_num_array_integer = 10,
        .hist_num_array_boolean = 10,
        .hist_num_array_decimal = 10,
        .hist_num_array_string = 10,
        .hist_num_array_object = 10,
        .hist_num_array_array = 10,

        .hist_num_object = 10
    };

    odsb_table_t *prop_table = odbs_table_by_prop(&props);
    srandom(time(NULL));

    for (int i = 0; i < 10; i++) {

        const void *random = odbs_table_random(prop_table);
        printf("%d\n", *(odsb_prop_type_e *) random);
    }

    odsb_histogram_t *histogram = odsb_histogram_new(3);
    odsb_histogram_key_add(histogram, "Hello World", 5);
    odsb_histogram_key_add(histogram, "Hello Mister", 5);
    odsb_histogram_key_add(histogram, "Max Mustermann", 5);
    odsb_histogram_key_add(histogram, "Hanna Montana", 100);


    odsb_histogram_t *val_int = odsb_histogram_new(3);
    odsb_histogram_t *val_boolean = odsb_histogram_new(3);
    odsb_histogram_t *val_decimal = odsb_histogram_new(3);
    odsb_histogram_t *val_string = odsb_histogram_new(3);
    odsb_histogram_t *array_int = odsb_histogram_new(3);
    odsb_histogram_t *array_decimal = odsb_histogram_new(3);
    odsb_histogram_t *array_boolean = odsb_histogram_new(3);
    odsb_histogram_t *array_string = odsb_histogram_new(3);
    odsb_histogram_t *array_array = odsb_histogram_new(3);
    odsb_histogram_t *array_object = odsb_histogram_new(3);
    odsb_histogram_t *object = odsb_histogram_new(3);

    odsb_histogram_key_add(val_int, "int-key_1", 10);
    odsb_histogram_key_add(val_int, "int-key_2", 20);
    odsb_histogram_key_add(val_int, "int-key_3", 30);

    odsb_histogram_key_add(val_boolean, "bool-key_1", 10);
    odsb_histogram_key_add(val_boolean, "bool-key_2", 20);
    odsb_histogram_key_add(val_boolean, "bool-key_3", 30);

    odsb_histogram_key_add(val_decimal, "dec-key_1", 10);
    odsb_histogram_key_add(val_decimal, "dec-key_2", 20);
    odsb_histogram_key_add(val_decimal, "dec-key_3", 30);

    odsb_histogram_key_add(val_string, "str-key_1", 10);
    odsb_histogram_key_add(val_string, "str-key_2", 20);
    odsb_histogram_key_add(val_string, "str-key_3", 30);

    odsb_histogram_key_add(array_int, "arr-int-key_1", 10);
    odsb_histogram_key_add(array_int, "arr-int-key_2", 20);
    odsb_histogram_key_add(array_int, "arr-int-key_3", 30);

    odsb_histogram_key_add(array_decimal, "arr-dec-key_1", 10);
    odsb_histogram_key_add(array_decimal, "arr-dec-key_2", 20);
    odsb_histogram_key_add(array_decimal, "arr-dec-key_3", 30);

    odsb_histogram_key_add(array_boolean, "arr-bool-key_1", 10);
    odsb_histogram_key_add(array_boolean, "arr-bool-key_2", 20);
    odsb_histogram_key_add(array_boolean, "arr-bool-key_3", 30);

    odsb_histogram_key_add(array_string, "arr-str-key_1", 10);
    odsb_histogram_key_add(array_string, "arr-str-key_2", 20);
    odsb_histogram_key_add(array_string, "arr-str-key_3", 30);

    odsb_histogram_key_add(array_array, "arr-arr-key_1", 10);
    odsb_histogram_key_add(array_array, "arr-arr-key_2", 20);
    odsb_histogram_key_add(array_array, "arr-arr-key_3", 30);

    odsb_histogram_key_add(array_object, "arr-obj-key_1", 10);
    odsb_histogram_key_add(array_object, "arr-obj-key_2", 20);
    odsb_histogram_key_add(array_object, "arr-obj-key_3", 30);

    odsb_histogram_key_add(object, "obj-key_1", 10);
    odsb_histogram_key_add(object, "obj-key_2", 20);
    odsb_histogram_key_add(object, "obj-key_3", 30);

    odsb_key_gen_params_t *key_gen = odsb_key_gen_new(val_int, val_boolean, val_decimal, val_string,
                                             array_int, array_boolean, array_decimal, array_string, array_array,
                                             array_object, object);


    for (int i = 0; i < 100; i++) {
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_integer));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_boolean));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_decimal));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_string));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_array_integer));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_array_boolean));
        printf("%s\n", odsb_key_gen_random(key_gen, prpo_type_array_decimal));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_array_string));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_array_object));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_array_array));
        printf("%s\n", odsb_key_gen_random(key_gen, prop_type_object));
    }





    odsb_histogram_t *values_val_int = odsb_histogram_new(3);
    odsb_histogram_t *values_val_boolean = odsb_histogram_new(3);
    odsb_histogram_t *values_val_decimal = odsb_histogram_new(3);
    odsb_histogram_t *values_val_string = odsb_histogram_new(3);
    /*odsb_histogram_t *values_array_int = odsb_histogram_new(3);
    odsb_histogram_t *values_array_decimal = odsb_histogram_new(3);
    odsb_histogram_t *values_array_boolean = odsb_histogram_new(3);
    odsb_histogram_t *values_array_string = odsb_histogram_new(3);
    odsb_histogram_t *values_array_array = odsb_histogram_new(3);
    odsb_histogram_t *values_array_object = odsb_histogram_new(3);
    odsb_histogram_t *values_object = odsb_histogram_new(3);*/

    odsb_histogram_value_int_add(values_val_int, 123, 10);
    odsb_histogram_value_int_add(values_val_int, 456, 20);
    odsb_histogram_value_int_add(values_val_int, 789, 30);

    odsb_histogram_value_boolean_add(values_val_boolean, true, 3);
    odsb_histogram_value_boolean_add(values_val_boolean, false, 1);

    odsb_histogram_value_double_add(values_val_decimal, 1.0, 10);
    odsb_histogram_value_double_add(values_val_decimal, 2.5, 20);
    odsb_histogram_value_double_add(values_val_decimal, 3.145, 30);

    odsb_histogram_value_string_add(values_val_string, "Value1", 10);
    odsb_histogram_value_string_add(values_val_string, "Value2", 20);
    odsb_histogram_value_string_add(values_val_string, "Value3", 30);

    odsb_key_gen_params_t *value_gen = odsb_key_gen_new(values_val_int, values_val_boolean, values_val_decimal, values_val_string,
                                                      NULL, NULL, NULL, NULL, NULL,
                                                        NULL, NULL);

    for (int i = 0; i < 100; i++) {
        printf("%llu\n", *(uint64_t *) odsb_value_gen_random(value_gen, prop_type_integer));
        printf("%d\n", *(bool *) odsb_value_gen_random(value_gen, prop_type_boolean));
        printf("%f\n", *(double *) odsb_value_gen_random(value_gen, prop_type_decimal));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_string));
        /*printf("%s\n", odsb_value_gen_random(value_gen, prop_type_array_integer));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_array_boolean));
        printf("%s\n", odsb_value_gen_random(value_gen, prpo_type_array_decimal));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_array_string));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_array_object));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_array_array));
        printf("%s\n", odsb_value_gen_random(value_gen, prop_type_object));*/
    }

    return 0;
}