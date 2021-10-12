/*
 * third prototype - linked-list logic
 * gcc pmacct_ptagmap_avro.p2.c -o pmacct_ptagmap_avro.p2 -lavro
 */

#include <string.h>
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
 *
 * code simplification - main blocks:
 *
 * - schema init/dump
 * - data load
 * - data print
 */


/* Global Variables */
char LABELS0[] = "node_id_key1,node_id_value1,platform_id_key1,platform_id_value1,"
                 "node_id_key2,node_id_value2,platform_id_key2,platform_id_value2";
char LABELS1[] = "node_id_key1,node_id_value1,platform_id_key1,platform_id_value1,"
                 "node_id_key2,node_id_value2,platform_id_key2,platform_id_value2";
const int MAX_TOCKENS = 256; //Max amount of tokens per string: 128 Labels
const char *DELIM = ",";
//
// --- AVRO global variables ---
//
avro_schema_t sc_type_record, sc_type_map, sc_type_string;
avro_value_t v_type_string, v_type_map, v_type_record;

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
//
// --- AVRO prototypes ---
//


int
main(void)
{
  struct label *start_w = labels_to_linked_list(LABELS0);
  struct label *start_r = labels_to_linked_list(LABELS1);
  struct label *ptr_w, *ptr_r;
  printf("start_w:\n");
  print_labels(start_w);
  printf("\nstart_r:\n");
  print_labels(start_r);
  printf("\n---\n");

  sc_type_string = avro_schema_string();
  sc_type_map = avro_schema_map(sc_type_string);
  sc_type_record = avro_schema_record("acct_data", NULL);
  avro_schema_record_field_append(sc_type_record, "label", sc_type_map);

  FILE *avro_schema_fp = NULL;
  avro_writer_t avro_schema_writer = NULL;
  avro_schema_fp = fopen("avro_schema.dump", "w");
  avro_schema_writer = avro_writer_file(avro_schema_fp);
  avro_schema_to_json(sc_type_record, avro_schema_writer);
  fclose(avro_schema_fp);

  avro_value_iface_t *if_type_record = avro_generic_class_from_schema(sc_type_record);
  avro_value_iface_t *if_type_map = avro_generic_class_from_schema(sc_type_map);
  avro_value_iface_t *if_type_string = avro_generic_class_from_schema(sc_type_string);

  avro_generic_value_new(if_type_record, &v_type_record);
  avro_generic_value_new(if_type_map, &v_type_map);
  avro_generic_value_new(if_type_string, &v_type_string);

  avro_file_writer_t db_w;
  const char *dbname = "avro_record.db";

  remove(dbname);
  int rval_w = avro_file_writer_create(dbname, sc_type_record, &db_w);
  if (rval_w)
  {
    fprintf(stderr, "Error: %s\n", avro_strerror());
    return -1;
	}

  size_t  map_size;
  avro_value_get_size(&v_type_map, &map_size);
  printf("before: %u\n", map_size);

  ptr_w = start_w;
  while (ptr_w != NULL)
  {
    if (avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL) == 0)
    {
      if (avro_value_add(&v_type_map, ptr_w->key, &v_type_string, NULL, NULL) == 0)
      {
        avro_value_set_string(&v_type_string, ptr_w->value);
        avro_file_writer_append_value(db_w, &v_type_record);
      }
    }
    ptr_w = ptr_w->next;
  }

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  avro_value_get_size(&v_type_map, &map_size);
  printf("after: %u\n", map_size);

  avro_file_reader_t db_r;
  int rval_r = avro_file_reader(dbname, &db_r);
  if (rval_r)
  {
    fprintf(stderr, "Error: %s\n", avro_strerror());
    return -1;
	}

  avro_file_reader_read_value(db_r, &v_type_record);
  size_t value_size;
  const char *p = NULL;

  ptr_r = start_r;
  while (ptr_r != NULL)
  {
    if (avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL) == 0)
    {
      if (avro_value_get_by_name(&v_type_map, ptr_r->key, &v_type_string, NULL) == 0)
      {
        avro_value_get_string(&v_type_string, &p, &value_size);
      }
    }
    //fprintf(stdout, "%u\n", value_size);
    fprintf(stdout, "%s:  %s\n", ptr_r->key, p);
    ptr_r = ptr_r->next;
  }

  avro_file_reader_close(db_r);

  free_labels(start_w);
  free_labels(start_r);
  avro_value_iface_decref(if_type_record); //no need to decref the associated value
  avro_value_iface_decref(if_type_map); //no need to decref the associated value
  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_schema_decref(sc_type_record);
  avro_schema_decref(sc_type_map);
  avro_schema_decref(sc_type_string);
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
    printf("%s: %s\n", ptr->key, ptr->value);
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
