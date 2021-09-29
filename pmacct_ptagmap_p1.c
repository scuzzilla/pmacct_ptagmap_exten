/*
 * second prototype - split logic
 * gcc pmacct_ptagmap_p1.c -o pmacct_ptagmap_p1 -ljansson
 */

#include <string.h>
#include <jansson.h>


/* Function prototypes */
char **split_label(char *, char **,  int);

/* Global Variables */
const int SET_LABEL_STACK = 2;
const char *DELIM = ",";


int
main(void)
{
  char label[] = "node_id_key,node_id_value,platform_id_key,platform_id_value";
  char *label_tokens_kv[SET_LABEL_STACK * 2];
  char **tkns = split_label(label, label_tokens_kv, SET_LABEL_STACK);

  char other1[] = "other1";
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
  json_decref(root_l1);

  return 0;
}


/*
 * Is receiving in input a "well formatted" string (master_label) resulting from the concatenation of set_label's
 * values configured within the pretag.map file; plus the integer referring to the amount of set_label declarations.
 * It's returning an array of *char each one of them pointing to a token generated from the master_label 
 */

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
