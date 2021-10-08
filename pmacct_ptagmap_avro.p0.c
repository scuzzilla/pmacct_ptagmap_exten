#include <avro.h>
#include <stdio.h>
#include <stdlib.h>


//const char  TEST_RECORD[] =
//"{\"type\":\"record\",\
//  \"name\":\"test_record\",\
//  \"fields\":[\
//    {\"name\": \"test_field\", \"type\": \"string\"}]}";


int
main(void)
{
  const char *t_value = "test_value";

  //avro_schema_t test_record;
  //avro_schema_from_json_literal(TEST_RECORD, &test_record);

  avro_schema_t test_record = avro_schema_record("test_record", NULL);
  avro_schema_record_field_append(test_record, "test_field", avro_schema_string());

  avro_value_iface_t *record_class = avro_generic_class_from_schema(test_record);
  avro_value_t test_value, test_record_value;
  avro_generic_value_new(record_class, &test_record_value);

  //printf("%s\n", t_value);

  if (avro_value_get_by_name(&test_record_value, "test_field", &test_value, NULL) == 0)
  {
    //printf("%s\n", t_value);
    avro_value_set_string(&test_value, t_value);
  }

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;

  avro_schema_fp = fopen("avro_schema.dump", "w");
	avro_schema_writer = avro_writer_file(avro_schema_fp);
  avro_schema_to_json(test_record, avro_schema_writer);

  fclose(avro_schema_fp);

	avro_file_writer_t db;
  const char *dbname = "avro_record.db";

  remove(dbname);
  avro_file_writer_create(dbname, test_record, &db);

  avro_file_writer_append_value(db, &test_record_value);
  avro_file_writer_flush(db);
  avro_file_writer_close(db);

  avro_file_reader_read_value(db, &test_record_value);
  size_t size;
  int32_t *p;
  avro_value_get_string(&test_value, &p, &size);
  fprintf(stdout, "%s\n", p);

  return 0;
}
