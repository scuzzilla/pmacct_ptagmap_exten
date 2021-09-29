/*
 * third prototype - linked-list logic
 * gcc pmacct_ptagmap_p2.c -o pmacct_ptagmap_p2 -ljansson */

#include <string.h>
#include <jansson.h>


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
/* void print_labels(struct label *); */
void free_labels(struct label *);
json_t *labels_to_json_obj(struct label *);


int
main(void)
{
  struct label *start = labels_to_linked_list(LABELS);

  json_t *root_l0 = json_object();
  json_t *root_l1 = labels_to_json_obj(start);
  json_t *j_str_tmp = NULL;

  json_object_set_new(root_l0, "label", root_l1);

  char *j_dump_final  = json_dumps(root_l0, JSON_INDENT(2));
  printf("%s\n", j_dump_final);
  free(j_dump_final);

  json_decref(root_l0);
  free_labels(start);

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

/*
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
*/

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

    idx_1 = idx_1 + 2;
  }

  return start;
}
