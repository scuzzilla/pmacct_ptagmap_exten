/* first prototype - gcc pmacct_ptagmap_p0.c -o pmacct_ptagmap_p0 -ljansson */

#include <string.h>
#include <jansson.h>


int
main(void)
{
  char label[] = "node_id_key,node_id_value,platform_id_key,platform_id_value";
  char other1[] = "other1";
  char other2[] = "other2";
  char other3[] = "other3";
  char *tkns[4] = {NULL, NULL, NULL, NULL};
  json_t *root_l0 = json_object();
  json_t *root_l1 = json_object();
  json_t *j_str_tmp = NULL;
  //printf("%s\n", str);

  int idx = 0;
  for (char *tkn = strtok(label,","); tkn != NULL; tkn = strtok(NULL, ","))
  {
    tkns[idx] = tkn;
    idx++;
  }

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
