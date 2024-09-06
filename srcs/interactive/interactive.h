#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <expert_system.h>

int interactive_mode(t_expert_system* es, bool is_verbose);
void print_tokens(t_token *tokens);

#define MENU_INTERACTIVE "Interactive mode\r\n\
    Commands:\r\n\
    - q: quit\r\n\
    - s: show all rules\r\n\
    - i: print initial values\r\n\
    - p: print queries\r\n\
    - r: remove rule by it's number\r\n\
    - n: add a new rule\r\n\
    - m: modify initial values\r\n\
    - k: modify queries\r\n\
    - e: evaluate actual rules.\r\n\
    - v: enable or disable verbose mode\r\n\
    - h: print this menu\r\n"


#endif