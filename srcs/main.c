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
#include <evaluate.h>

int main(int argc, char**argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return INVALID_ARGS;
    }

    t_expert_system es;

    parse(argv[1], &es);

    evaluate(&es);

    free_rules(es.rules);
    free_rules(es.initial_values);
    free_rules(es.queries);
    return OK;
}