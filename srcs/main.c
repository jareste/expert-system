#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>
#include <parser.h>
#include <evaluate.h>
#include <interactive.h>

int main(int argc, char**argv)
{
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: %s <filename> [-i]\r\n", argv[0]);
        return INVALID_ARGS;
    }

    srand(time(NULL));

    t_expert_system es;

    parse(argv[1], &es);

    if (argc == 3)
    {
        if (strcmp(argv[2], "-i") != 0)
        {
            fprintf(stderr, "Usage: %s <filename> [-i]\r\n", argv[0]);
            return INVALID_ARGS;
        }
        return interactive_mode(&es);
    }

    evaluate(&es);

    free_rules(es.rules);
    free_rules(es.initial_values);
    free_rules(es.queries);
    return 0;
}