
#include <brooks/brooks_doc.h>
#include <brooks/brooks_query.h>
#include <brooks/query/operators/scans/brooks_scan_objects.h>
#include <brooks/query/brooks_cursor.h>
#include <brooks/query/operators/scans/brooks_scan_arrays.h>

/*

 {
    "snapshot_date": "Oct 23th, 2017",
    "source"={
        "site": "http://www.imdb.com/title/tt1396484/?ref_=nv_sr_1"
    },
    "movies"=[{
        "title": "It (2017)",
        "actors": [{ "name": "Bill Skarsgård", "role": "Pennywise" }, { "name": "Jaeden Lieberher", "role": "Bill" }],
        "keywords": ["clown", "based on novel", "supernatural", "balloon", "fear"],
        "poster_url": null,
        "reviews": 928
        "rating": 7.8
    }, {
        "title": "Jigsaw (2017)",
        "actors": [{ "name": "Tobin Bell", "role": "John Kramer" }, { "name": "Matt Passmore", "role": "Logan Nelson" }],
        "keywords": ["copycat killer", "one word title", "cop", "murder investigation"],
        "poster_url": "https://images-na.ssl-images-amazon.com/images/M/MV5BNmRiZDM4ZmMtOTVjMi00YTNlLTkyNjMtMjI2OTAxNjgwMWM1XkEyXkFqcGdeQXVyMjMxOTE0ODA@._V1_SY1000_CR0,0,648,1000_AL_.jpg"
    }]
 }

 */

bool pred_objects_or_arrays_only(void *capture, const brooks_element_t *element)
{
    brooks_type_e type;
    brooks_doc_element_get_type(&type, element);
    return (type == brooks_type_object || type == brooks_type_array);
}

int main(int argc, char* argv[])
{
    brooks_pool_t *pool;
    brooks_object_t *document, *document2, *source, *it, *jigsaw, *actors_skarsgard, *actors_lieberher, *actors_bell, *actors_passmore;
    brooks_array_t *movies, *it_actors, *it_keywords, *jigsaw_actors, *jigsaw_keywords;

    brooks_pool_create(&pool);
    brooks_doc_create(&document, pool);
    for (int i = 0; i < 1; i++) {
        brooks_doc_add_string(document, "snapshot_date", "Oct 23th, 2017");
        brooks_doc_add_object(&source, document, "source");
        brooks_doc_add_string(source, "site", "http://www.imdb.com/title/tt1396484/?ref_=nv_sr_1");
        for (int j = 0; j < 2; j++) {
            brooks_doc_add_array(&movies, document, brooks_type_object, "movies");
            brooks_doc_array_add_object(&it, movies);
            brooks_doc_add_string(it, "title", "It (2017)");
            brooks_doc_add_array(&it_actors, it, brooks_type_object, "actors");
            brooks_doc_array_add_object(&actors_skarsgard, it_actors);
            brooks_doc_add_string(actors_skarsgard, "name", "Bill Skarsgård");
            brooks_doc_add_string(actors_skarsgard, "role", "Pennywise");
            brooks_doc_array_add_object(&actors_lieberher, it_actors);
            brooks_doc_add_string(actors_lieberher, "name", "Jaeden Lieberher");
            brooks_doc_add_string(actors_lieberher, "role", "Bill");
        }
    }
    brooks_doc_add_array(&it_keywords, it, brooks_type_string, "keywords");
    brooks_doc_add_value(it_keywords, "clown");
    brooks_doc_add_value(it_keywords, "based on novel");
    brooks_doc_add_value(it_keywords, "supernatural");
    brooks_doc_add_value(it_keywords, "balloon");
    brooks_doc_add_value(it_keywords, "fear");
    brooks_doc_add_null(it, "poster_url");
    brooks_doc_add_integer(it, "reviews", &(uint64_t) {928});
    brooks_doc_add_decimal(it, "rating", &(double) {7.8});
    brooks_doc_array_add_object(&jigsaw, movies);
    brooks_doc_add_string(jigsaw, "title", "Jigsaw (2017)");
    brooks_doc_add_array(&jigsaw_actors, jigsaw, brooks_type_object, "actors");
    brooks_doc_array_add_object(&actors_bell, jigsaw_actors);
    brooks_doc_add_string(actors_bell, "name", "Tobin Bell");
    brooks_doc_add_string(actors_bell, "role", "John Kramer");
    brooks_doc_array_add_object(&actors_passmore, jigsaw_actors);
    brooks_doc_add_string(actors_passmore, "name", "Matt Passmore");
    brooks_doc_add_string(actors_passmore, "role", "Logan Nelson");
    brooks_doc_add_array(&jigsaw_keywords, jigsaw, brooks_type_string, "keywords");
    brooks_doc_add_value(jigsaw_keywords, "copycat killer");
    brooks_doc_add_value(jigsaw_keywords, "one word title");
    brooks_doc_add_value(jigsaw_keywords, "cop");
    brooks_doc_add_value(jigsaw_keywords, "murder investigation");
    brooks_doc_add_string(jigsaw, "poster_url",
                            "https://images-na.ssl-images-amazon.com/images/M/MV5BNmRiZDM4ZmMtOTVjMi00YTNlLTkyNjMtMjI2OTAxNjgwMWM1XkEyXkFqcGdeQXVyMjMxOTE0ODA@._V1_SY1000_CR0,0,648,1000_AL_.jpg");

    printf("JSON printed:\n\t");
    brooks_doc_print(stdout, document);
    fprintf(stdout, "\n\n\n");

    const char *text = "{ \"snapshot_date\": \"Oct 23th, 2017\", \"source\": { \"site\": \"http://www.imdb.com/title/tt1396484/?ref_=nv_sr_1\" }, \"movies\": [ { \"title\": \"It (2017)\", \"actors\": [ { \"name\": \"Bill Skarsgård\", \"role\": \"Pennywise\" }, { \"name\": \"Jaeden Lieberher\", \"role\": \"Bill\" } ] } ], \"movies\": [ { \"title\": \"It (2017)\", \"actors\": [ { \"name\": \"Bill Skarsgård\", \"role\": \"Pennywise\" }, { \"name\": \"Jaeden Lieberher\", \"role\": \"Bill\" } ], \"keywords\": [ \"clown\", \"based on novel\", \"supernatural\", \"balloon\", \"fear\" ], \"poster_url\": null, \"reviews\": 928, \"rating\": 7.800000 }, { \"title\": \"Jigsaw (2017)\", \"actors\": [ { \"name\": \"Tobin Bell\", \"role\": \"John Kramer\" }, { \"name\": \"Matt Passmore\", \"role\": \"Logan Nelson\" } ], \"keywords\": [ \"copycat killer\", \"one word title\", \"cop\", \"murder investigation\" ], \"poster_url\": \"https://images-na.ssl-images-amazon.com/images/M/MV5BNmRiZDM4ZmMtOTVjMi00YTNlLTkyNjMtMjI2OTAxNjgwMWM1XkEyXkFqcGdeQXVyMjMxOTE0ODA@._V1_SY1000_CR0,0,648,1000_AL_.jpg\" } ] }";
    brooks_doc_parse(document2, text);

/*
    brooks_operator_t scan_object, scan_array;

    brooks_operators_scan_objects_create(&scan_object, &document, 1, pool);
    brooks_operator_open(&scan_object);
    const brooks_cursor_t *cursor_object, *cursor_array;

    while ((cursor_object = brooks_operator_next(&scan_object)) != NULL) {
        size_t num_elements;
        brooks_value_t **base = brooks_cursor_read(&num_elements, cursor_object);
        while (num_elements--) {
            brooks_doc_value_print(stdout, *base);
            fprintf(stdout, "\n");

            brooks_type_e type;
            if (brooks_doc_value_get_type(&type, *base) && (type == brooks_type_array)) {
                brooks_array_t *array = brooks_doc_value_as_array(*base);
                brooks_operators_scan_arrays_create(&scan_array, &array, 1, pool);
                brooks_operator_open(&scan_array);
                while ((cursor_array = brooks_operator_next(&scan_array)) != NULL) {
                    size_t num_elements;
                    brooks_value_t **base = brooks_cursor_read(&num_elements, cursor_array);
                    while (num_elements--) {
                        fprintf(stdout, "\t");
                        brooks_doc_value_print(stdout, *base);
                        fprintf(stdout, "\n");
                    }
                }
            }

            base++;
        }
    }





    brooks_operator_close(&scan_object);
*/
    //brooks_query_t *query;
    //brooks_result_t *result;
    //xjson_query_start(&query, pool, document, NULL, pred_objects_or_arrays_only);
    //xjson_query_print(stdout, query);

   // brooks_query_create(&query, pool);
   // brooks_query_execute(&result, pool, query, document, brooks_traversal_breadth_first);
  //  brooks_result_print(stdout, result);


    brooks_pool_dispose(pool);


}