/*
 * third prototype - linked-list logic
 * gcc pmacct_ptagmap_avro.p2.c -o pmacct_ptagmap_avro.p2 -lavro
 */

#include <string.h>
#include <avro.h>
#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>


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


/* Global Variables */
char LABELS[] = "node_id_key1,node_id_value1,platform_id_key1,platform_id_value1,"
                "node_id_key2,node_id_value2,platform_id_key2,platform_id_value2";
const int MAX_TOCKENS = 256; //Max amount of tokens per string: 128 Labels
const char *DELIM = ",";

/* Data structures */
struct label {
  char *key;
  char *value;
  struct label *next;
};

/* Function prototypes - linked-list logic */
struct label *create_label(char *, char *);
struct label *append_label(struct label *, struct label *);
struct label *labels_to_linked_list(char *);
void print_labels(struct label *);
void free_labels(struct label *);
json_t *labels_to_json_obj(struct label *);


int
main(void)
{
  struct label *start = labels_to_linked_list(LABELS);
  print_labels(start);

  json_t *root_l0 = json_object();
  json_t *root_l1 = labels_to_json_obj(start);
  json_t *j_str_tmp = NULL;

  json_object_set_new(root_l0, "label", root_l1);

  char *j_dump_final  = json_dumps(root_l0, JSON_INDENT(2));
  printf("%s\n", j_dump_final);
  free(j_dump_final);

  json_decref(root_l0);
  free_labels(start);

  avro_value_t v_type_string;
  avro_value_t v_type_map;
  avro_value_t v_type_record;

  avro_schema_t sc_type_string = avro_schema_string();
  avro_schema_t sc_type_map = avro_schema_map(sc_type_string);
  avro_schema_t sc_type_record = avro_schema_record("acct_data", NULL);

  avro_schema_record_field_append(sc_type_record, "label", sc_type_map);

  avro_value_iface_t *if_type_record = avro_generic_class_from_schema(sc_type_record);
  avro_value_iface_t *if_type_map = avro_generic_class_from_schema(sc_type_map);
  avro_value_iface_t *if_type_string = avro_generic_class_from_schema(sc_type_string);
  avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_map, &v_type_map);
  avro_generic_value_new(if_type_string, &v_type_string);

  avro_file_writer_t db_w;
  const char *dbname = "avro_record.db";

  remove(dbname);
  avro_file_writer_create(dbname, sc_type_record, &db_w);

  size_t  map_size;
  avro_value_get_size(&v_type_map, &map_size);
  printf("before: %u\n", map_size);

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_add(&v_type_map, "sc_map_key", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value");
  avro_file_writer_append_value(db_w, &v_type_record);

  //avro_value_reset(&v_type_string);
  //avro_value_reset(&v_type_record);

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_add(&v_type_map, "sc_map_key1", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value1");
  avro_file_writer_append_value(db_w, &v_type_record);

  //avro_value_reset(&v_type_string);
  //avro_value_reset(&v_type_record);

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_add(&v_type_map, "sc_map_key2", &v_type_string, NULL, NULL);
  avro_value_set_string(&v_type_string, "sc_map_value2");
  avro_file_writer_append_value(db_w, &v_type_record);

  //avro_value_reset(&v_type_string);
  //avro_value_reset(&v_type_record);

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  avro_value_get_size(&v_type_map, &map_size);
  printf("after: %u\n", map_size);

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;

  avro_schema_fp = fopen("avro_schema.dump", "w");
  avro_schema_writer = avro_writer_file(avro_schema_fp);
  avro_schema_to_json(sc_type_record, avro_schema_writer);
  fclose(avro_schema_fp);

  //--------------------------------------------

  avro_file_reader_t db_r;
  avro_file_reader_read_value(db_r, &v_type_record);
  int rval = avro_file_reader(dbname, &db_r);
  size_t value_size;
  const char *p = NULL;

  if (rval)
  {
    fprintf(stderr, "Error: %s\n", avro_strerror());
    return -1;
	}

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_get_by_name(&v_type_map, "sc_map_key", &v_type_string, NULL);
  avro_value_get_string(&v_type_string, &p, &value_size);
  fprintf(stdout, "%u\n", value_size);
  fprintf(stdout, "%s\n", p);

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_get_by_name(&v_type_map, "sc_map_key1", &v_type_string, NULL);
  avro_value_get_string(&v_type_string, &p, &value_size);
  fprintf(stdout, "%u\n", value_size);
  fprintf(stdout, "%s\n", p);

  avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL);
  avro_value_get_by_name(&v_type_map, "sc_map_key2", &v_type_string, NULL);
  avro_value_get_string(&v_type_string, &p, &value_size);
  fprintf(stdout, "%u\n", value_size);
  fprintf(stdout, "%s\n", p);

  avro_file_reader_close(db_r);

  avro_value_iface_decref(if_type_record); //no need to decref the associated value
  avro_value_iface_decref(if_type_map); //no need to decref the associated value
  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_schema_decref(sc_type_map);
  avro_schema_decref(sc_type_string);
  avro_schema_decref(sc_type_record);
  //avro_value_decref(&v_type_record);
  //avro_value_decref(&v_type_map);
  //avro_value_decref(&v_type_string);

  return 0;
}


struct label *
create_label(char *tkn_key, char *tkn_value)
{
  struct label *ptr;
  ptr = (struct label *) malloc(sizeof(struct label));
  ptr->key = tkn_key;
  ptr->value = tkn_value;
  ptr->next = NULL;
  return ptr;
}

struct label *
append_label(struct label *end, struct label *new_label_ptr)
{
  end->next = new_label_ptr;
  return(end->next);
}

void
print_labels(struct label *start)
{
  struct label *ptr;
  ptr = start;
  while(ptr != NULL) {
    printf("%s -> %s\n", ptr->key, ptr->value);
    ptr = ptr->next;
  }
}

void
free_labels(struct label *start)
{
  struct label *ptr = start;
  struct label *tmp;
  while (ptr!=NULL) {
    tmp = ptr->next;
    free(ptr);
    ptr = tmp;
  }
}

json_t *
labels_to_json_obj(struct label *start)
{
  struct label *ptr;
  ptr = start;
  json_t *root = json_object();
  json_t *j_str_tmp = NULL;

  while(ptr != NULL) {
    j_str_tmp = json_string(ptr->value);
    json_object_set_new(root, ptr->key, j_str_tmp);
    ptr = ptr->next;
  }

  return root;
}

struct label *
labels_to_linked_list(char *label)
{
  char *token = NULL;
  char *tokens[MAX_TOCKENS];
  struct label *start, *new_label_ptr, *end;

  int idx_0 = 0;
  for (token = strtok(label, DELIM); token != NULL; token = strtok(NULL, DELIM))
  {
    tokens[idx_0] = token;
    idx_0++;
  }

  /* Head of the labels' linked list */
  start = create_label(tokens[0], tokens[1]);
  end = start;

  int idx_1 = 2;
  while (idx_1 < idx_0)
  {
    new_label_ptr = create_label(tokens[idx_1], tokens[idx_1 + 1]);
    end = append_label(end, new_label_ptr);

    idx_1 += 2;
  }

  return start;
}
