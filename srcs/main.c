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
    if (argc < 2 || argc > 4)
    {
        fprintf(stderr, "Usage: %s <filename> [-i] [-v]\r\n", argv[0]);
        return INVALID_ARGS;
    }

    srand(time(NULL));

    t_expert_system es;
    int interactive = 0;
    int verbose = 0;

    parse(argv[1], &es);

    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            interactive = 1;
        }
        else if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }
        else
        {
            fprintf(stderr, "Usage: %s <filename> [-i] [-v]\r\n", argv[0]);
            return INVALID_ARGS;
        }
    }

    if (interactive)
    {
        return interactive_mode(&es, verbose);
    }

    evaluate(&es, verbose);


    free_rules(es.rules);
    free_rules(es.initial_values);
    free_rules(es.queries);
    return 0;
}