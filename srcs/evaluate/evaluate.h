#ifndef EVALUATE_H
#define EVALUATE_H

#include <expert_system.h>

int evaluate(t_expert_system *rules, bool is_verbose);
uint get_initial_value(t_token* token);
void print_marked_letters(uint value);

#endif