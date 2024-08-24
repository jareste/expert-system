#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>
#include <parser.h>

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return INVALID_ARGS;
    }

    t_rule *rules = NULL;

    parse(argv[1], &rules);


    free_rules(rules);
    return OK;
}