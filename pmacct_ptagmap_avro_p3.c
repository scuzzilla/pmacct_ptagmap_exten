/*
 * fourth prototype - linked-list logic with libcdada (AVRO version)
 *
 * 1. Invoke generator with the types you want to support
 *    ~/Projects/libcdada/tools/cdada-gen list:ptm_label -o ~/Projects/pmacct_ptagmap_exten/autogen_cdada_ptm_label.cc
 *
 * 2. Add header includes for types 'ptm_label' in the place holder
 *    vim ~/Projects/pmacct_ptagmap_exten/autogen_cdada_ptm_label.cc
 *
 * 3. Add to your build system
 *    g++ -c ~/Projects/pmacct_ptagmap_exten/autogen_cdada_ptm_label.cc
 *
 * 4. Link your application; make sure to link against -lcdada:
 *    cd ~/Projects/pmacct_ptagmap_exten/
 *    gcc pmacct_ptagmap_avro_p3.c autogen_cdada_ptm_label.o -o bin/pmacct_ptagmap_avro_p3 -lcdada -lstdc++ -lavro
 */

#include <string.h>
#include <avro.h>
#include <stdio.h>
#include <stdlib.h>
#include <cdada/list.h>
#include <cdada/str.h>
#include "cdada_types/ptm_label.h"

CDADA_LIST_CUSTOM_TYPE_DECL(ptm_label);


/* Global Variables */
char LABELS[] = "node_id_key1-node_id_value1,platform_id_key1-platform_id_value1,"
                "node_id_key2-node_id_value2,platform_id_key2-platform_id_value2";
const int MAX_TOCKENS = 256;
const char *DELIM = ",";
//
// --- AVRO global variables ---
//
avro_schema_t sc_type_record, sc_type_map, sc_type_string;
avro_value_t v_type_string, v_type_map, v_type_record;
avro_value_iface_t *if_type_record, *if_type_map, *if_type_string;

/* Function prototypes */
cdada_list_t *ptm_labels_to_linked_list(char *);
//
// --- AVRO prototypes ---
//
void compose_label_avro_schema(void);
int compose_label_avro_data(cdada_list_t *, int);
int print_label_avro_data(cdada_list_t *, int);
void free_label_avro_data_memory(void);


int
main(void)
{
  /* labels normalization */
  cdada_str_t *lbls_cdada = cdada_str_create(LABELS);
  cdada_str_replace_all(lbls_cdada, "-", DELIM);
  char *lbls_norm = cdada_str(lbls_cdada);

  /* linked-list creation */
  ptm_label lbl;
  cdada_list_t *ptm_ll = ptm_labels_to_linked_list(lbls_norm);
  int ll_size = cdada_list_size(ptm_ll);

  compose_label_avro_schema();
  compose_label_avro_data(ptm_ll, ll_size);
  print_label_avro_data(ptm_ll, ll_size);
  free_label_avro_data_memory();

  cdada_list_destroy(ptm_ll);

  return 0;
}


/* pre-tag map labels to linked-list*/
cdada_list_t *
ptm_labels_to_linked_list(char *ptm_labels)
{
  cdada_list_t *ptm_linked_list = cdada_list_create_custom(ptm_label);
  ptm_label lbl;

  char *token = NULL;
  char *tokens[MAX_TOCKENS];

  int tokens_counter = 0;
  for (token = strtok(ptm_labels, DELIM); token != NULL; token = strtok(NULL, DELIM))
  {
    tokens[tokens_counter] = token;
    tokens_counter++;
  }

  int list_counter;
  for (list_counter = 0; list_counter < tokens_counter; list_counter+=2)
  {
    memset(&lbl, 0, sizeof(lbl));
    lbl.key = tokens[list_counter];
    lbl.value = tokens[list_counter + 1];
    cdada_list_push_back(ptm_linked_list, &lbl);
  }

  return ptm_linked_list;
}
//
// --- AVRO functions ---
//
void
compose_label_avro_schema(void)
{
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
}


int
compose_label_avro_data(cdada_list_t *ll, int ll_size)
{
  ptm_label lbl;

  printf("start -> linked-list:\n");
  int idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    cdada_list_get(ll,idx_0,&lbl);
    printf("key: %s\n", lbl.key);
    printf("value: %s\n", lbl.value);
  }
  printf("\n\n---\n");

  if_type_record = avro_generic_class_from_schema(sc_type_record);
  if_type_map = avro_generic_class_from_schema(sc_type_map);
  if_type_string = avro_generic_class_from_schema(sc_type_string);

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

  int idx_1;
  for (idx_1 = 0; idx_1 < ll_size; idx_1++)
  {
    cdada_list_get(ll, idx_1, &lbl);
    if (avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL) == 0)
    {
      if (avro_value_add(&v_type_map, lbl.key, &v_type_string, NULL, NULL) == 0)
      {
        avro_value_set_string(&v_type_string, lbl.value);
        avro_file_writer_append_value(db_w, &v_type_record);
      }
    }
  }

  avro_file_writer_flush(db_w);
  avro_file_writer_close(db_w);

  avro_value_get_size(&v_type_map, &map_size);
  printf("after: %u\n", map_size);

  return 0;
}


int
print_label_avro_data(cdada_list_t *ll, int ll_size)
{
  ptm_label lbl;

  const char *dbname = "avro_record.db";
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

  int idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    cdada_list_get(ll, idx_0, &lbl);
    if (avro_value_get_by_name(&v_type_record, "label", &v_type_map, NULL) == 0)
    {
      if (avro_value_get_by_name(&v_type_map, lbl.key, &v_type_string, NULL) == 0)
      {
        avro_value_get_string(&v_type_string, &p, &value_size);
      }
    }
    //fprintf(stdout, "%u\n", value_size);
    fprintf(stdout, "%s:  %s\n", lbl.key, p);
  }

  avro_file_reader_close(db_r);

  return 0;
}


void
free_label_avro_data_memory(void)
{
  avro_value_iface_decref(if_type_record); //no need to decref the associated value
  avro_value_iface_decref(if_type_map); //no need to decref the associated value
  avro_value_iface_decref(if_type_string); //no need to decref the associated value
  avro_schema_decref(sc_type_record);
  avro_schema_decref(sc_type_map);
  avro_schema_decref(sc_type_string);
  //avro_value_decref(&v_type_record);
  //avro_value_decref(&v_type_map);
  //avro_value_decref(&v_type_string);
}
