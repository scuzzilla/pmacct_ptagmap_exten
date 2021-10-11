#include <avro.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * 1. schema string init -> avro_schema_t sc_type_string | sc_map_value
 * 2. schema map init    -> avro_schema_t sc_type_map    | sc_map_key, sc_map_value
 * 3. schema record init -> avro_schema_t sc_type_record | sc_rcrd_name, sc_rcrd_space, sc_rcrd_field_name
 * ---------------------
 * 1. append schema string to schema map
 * 2. append schema map to schema record
 * -------------------------------------
 * 1. create an iface to the record type (my assumption is that it should be enough)
 * ---------------------------------------------------------------------------------
 * 1. add value string to the map -> avro_value_t v_type_string
 * 2. add value map to the record -> avro_value_t v_type_map
 */


int
main(void)
{
  avro_value_t v_type_string;
  avro_value_t v_type_map;
  //avro_value_t v_type_record;

  avro_schema_t sc_type_string = avro_schema_string();
  avro_schema_t sc_type_map = avro_schema_map(sc_type_string);
  //avro_schema_t sc_type_record = avro_schema_record("sc_rcrd_name", "sc_rcrd_space");

  //avro_schema_record_field_append(sc_type_record, "sc_rcrd_field_name", sc_type_map);

  //avro_value_iface_t *if_type_record = avro_generic_class_from_schema(sc_type_record);
  avro_value_iface_t *if_type_map = avro_generic_class_from_schema(sc_type_map);
  avro_value_iface_t *if_type_string = avro_generic_class_from_schema(sc_type_string);
  //avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_map, &v_type_map);
  avro_generic_value_new(if_type_string, &v_type_string);

  avro_file_writer_t db_w;
  const char *dbname = "avro_record.db";

  remove(dbname);
  //avro_file_writer_create(dbname, sc_type_record, &db);
  avro_file_writer_create(dbname, sc_type_map, &db_w);

  //avro_file_writer_append_value(db_w, &v_type_record);

  size_t  map_size;
  avro_value_get_size(&v_type_map, &map_size);
  printf("before: %u\n", map_size);

  avro_value_add(&v_type_map, "sc_map_key", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value");
  avro_file_writer_append_value(db_w, &v_type_map);

  avro_value_reset(&v_type_string);

  avro_value_add(&v_type_map, "sc_map_key1", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value1");
  avro_file_writer_append_value(db_w, &v_type_map);

  avro_value_reset(&v_type_string);

  avro_value_add(&v_type_map, "sc_map_key2", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value2");
  avro_file_writer_append_value(db_w, &v_type_map);

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  //if (avro_value_get_by_name(&v_type_map, "sc_map_key", &v_type_string, NULL) == 0)
  //{
  //  printf("Inside\n");
  //  avro_value_add(&v_type_map, "sc_map_key", &v_type_string, NULL, NULL);
  //}

  avro_value_get_size(&v_type_map, &map_size);
  printf("after: %u\n", map_size);

  //if (avro_value_get_by_name(&v_type_record, "sc_rcrd_field_name", &v_type_map, NULL) == 0)
  //{
  //  avro_value_add(&v_type_map, "sc_map_key", &v_type_string, NULL, NULL);
  //}

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;

  avro_schema_fp = fopen("avro_schema.dump", "w");
  avro_schema_writer = avro_writer_file(avro_schema_fp);
  //avro_schema_to_json(sc_type_record, avro_schema_writer);
  avro_schema_to_json(sc_type_map, avro_schema_writer);
  fclose(avro_schema_fp);

  //--------------------------------------------

  avro_file_reader_t db_r;
  avro_file_reader_read_value(db_r, &v_type_map);
  size_t value_size;
  int32_t *p;

  if (avro_value_get_by_name(&v_type_map, "sc_map_key", &v_type_string, NULL) == 0)
  {
    avro_value_get_string(&v_type_string, &p, &value_size);
    fprintf(stdout, "%u\n", value_size);
    fprintf(stdout, "%s\n", p);
  }

  //avro_file_reader_close(db_r);

  //avro_value_iface_decref(if_type_record); //no need to decref the associated value
  avro_value_iface_decref(if_type_map); //no need to decref the associated value
  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_schema_decref(sc_type_map);
  avro_schema_decref(sc_type_string);
  //avro_schema_decref(sc_type_record);
  //avro_value_decref(&v_type_string);
  //avro_generic_value_free(&val)

  return 0;
}
