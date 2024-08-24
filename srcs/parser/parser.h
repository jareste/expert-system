#ifndef PARSE_H
#define PARSE_H

/* UTILS */
uint get_token_value(t_token *token);
void free_rules(t_rule *rules);

int parse(char* filename, t_expert_system *rules);

#endif