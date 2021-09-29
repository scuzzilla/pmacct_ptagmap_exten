/*
 * third prototype - linked-list logic
 * gcc pmacct_ptagmap_p2.c -o pmacct_ptagmap_p2 -ljansson */

#include <string.h>
#include <jansson.h>


/* Data structures */
struct label {
  char *key;
  char *value;
  struct label *next;
};

/* Function prototypes - split logic */
char **split_label(char *, char **,  int);

/* Function prototypes - linked-list logic */
struct label *create_label(char *, char *);
struct label *append_label(struct label *, struct label *);
void print_labels(struct label *);

/* Global Variables */
const int SET_LABEL_STACK = 2;
const int MAX_STACKED_LABELS = 256;
const char *DELIM = ",";


int
main(void)
{
  char label[] = "node_id_key,node_id_value,platform_id_key,platform_id_value";
  char *token = NULL;
  char *tokens[MAX_STACKED_LABELS];
  struct label *start, *new_label_ptr, *end, *tmp;

  //char *label_tokens_kv[SET_LABEL_STACK * 2];
  //char **tkns = split_label(label, label_tokens_kv, SET_LABEL_STACK);

  int idx_0 = 0;
  for (token = strtok(label, DELIM); token != NULL; token = strtok(NULL, DELIM))
  {
    tokens[idx_0] = token;
    idx_0++;
  }

  start = create_label(tokens[0], tokens[1]);
  end = start;

  new_label_ptr = create_label(tokens[2], tokens[3]);
  end = append_label(end, new_label_ptr);

  print_labels(start);
  tmp = start->next;

  free(start);

  start = tmp;
  tmp = start->next;

  free(start);
  free(tmp);

  /*char other1[] = "other1";
  char other2[] = "other2";
  char other3[] = "other3";

  json_t *root_l0 = json_object();
  json_t *root_l1 = json_object();
  json_t *j_str_tmp = NULL;
  //printf("%s\n", str);

  //printf("%s\n", tkns[0]);
  //printf("%s\n", tkns[1]);
  //printf("%s\n", tkns[2]);
  //printf("%s\n", tkns[3]);

  j_str_tmp = json_string(other1);
  json_object_set_new(root_l0, "other1", j_str_tmp);

  j_str_tmp = json_string(other2);
  json_object_set_new(root_l0, "other2", j_str_tmp);

  j_str_tmp = json_string(other3);
  json_object_set_new(root_l0, "other3", j_str_tmp);

  j_str_tmp = json_string(tkns[1]);
  json_object_set_new(root_l1, tkns[0], j_str_tmp);

  j_str_tmp = json_string(tkns[3]);
  json_object_set_new(root_l1, tkns[2], j_str_tmp);

  char *j_dump_l0  = json_dumps(root_l0, JSON_INDENT(2));
  printf("%s\n", j_dump_l0);
  free(j_dump_l0);

  json_object_set_new(root_l0, "label", root_l1);

  char *j_dump_l1  = json_dumps(root_l1, JSON_INDENT(2));
  printf("%s\n", j_dump_l1);
  free(j_dump_l1);

  char *j_dump_final  = json_dumps(root_l0, JSON_INDENT(2));
  printf("%s\n", j_dump_final);
  free(j_dump_final);

  json_decref(root_l0);
  json_decref(root_l1);*/

  return 0;
}


/* Is receiving in input a "well formatted" string (master_label) resulting from the concatenation of set_label's
 * values configured within the pretag.map file; plus the integer referring to the amount of set_label declarations.
 * It's returning an array of *char each one of them pointing to a token generated from the master_label */

char **split_label(char *label, char **tokens, int set_label_amount)
{
  tokens[set_label_amount * 2];

  int idx = 0;
  for (char *token = strtok(label, DELIM); token != NULL; token = strtok(NULL, DELIM))
  {
    tokens[idx] = token;
    idx++;
  }

  return tokens;
}

struct label *create_label(char *tkn_key, char *tkn_value) {
  struct label *ptr;
  ptr = (struct label *) malloc(sizeof(struct label));
  ptr->key = tkn_key;
  ptr->value = tkn_value;
  ptr->next = NULL;
  return ptr;
}

struct label *append_label(struct label * end, struct label * new_label_ptr) {
  end->next = new_label_ptr;
  return(end->next);
}

void print_labels(struct label *start)
{
	struct label *ptr;
  ptr = start;

	while(ptr != NULL) {
		printf("%s: %s\n", ptr->key, ptr->value);
		ptr = ptr->next;
	}
}
