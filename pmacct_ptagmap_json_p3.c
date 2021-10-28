
/*
 * fourth prototype - linked-list logic with libcdada (JSON version)
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
 *    gcc pmacct_ptagmap_json_p3.c autogen_cdada_ptm_label.o -o bin/pmacct_ptagmap_json_p3 -lcdada -lstdc++ -ljansson
 */

#include <string.h>
#include <jansson.h>
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

/* Function prototypes */
cdada_list_t *ptm_labels_to_linked_list(char *);
//
// --- JSON prototypes ---
//
json_t *compose_label_json_data(cdada_list_t *ll, int ll_size);


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

  json_t *root_l0 = json_object();
  json_t *root_l1 = compose_label_json_data(ptm_ll, ll_size);

  json_object_set_new(root_l0, "label", root_l1);

  char *j_dump_final  = json_dumps(root_l0, JSON_INDENT(2));
  printf("%s\n", j_dump_final);
  free(j_dump_final);

  json_decref(root_l0);
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
// --- JSON functions ---
//
json_t *
compose_label_json_data(cdada_list_t *ll, int ll_size)
{
  ptm_label lbl;

  printf("start -> linked-list:\n");
  int idx_0;
  for (idx_0 = 0; idx_0 < ll_size; idx_0++)
  {
    cdada_list_get(ll, idx_0, &lbl);
    printf("key: %s\n", lbl.key);
    printf("value: %s\n", lbl.value);
  }
  printf("\n\n---\n");

  json_t *root = json_object();
  json_t *j_str_tmp = NULL;

  int idx_1;
  for (idx_1 = 0; idx_1 < ll_size; idx_1++)
  {
    cdada_list_get(ll, idx_1, &lbl);
    j_str_tmp = json_string(lbl.value);
    json_object_set_new(root, lbl.key, j_str_tmp);
  }

  return root;
}
