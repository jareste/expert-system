#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>
#include <stdio.h>
#include <parser.h>

uint get_initial_value(t_token* token)
{
    t_token* current_token = token;
    uint value = 0;

    while (current_token)
    {
        if (current_token->type == LETTER)
        {
            value |= current_token->value;
        }
        current_token = FT_LIST_GET_NEXT(&token, current_token);
    }
    return value;
}

int evaluate(t_expert_system *rules)
{
    printf("Evaluating rules...\n");

    uint init_value = get_initial_value(rules->initial_values->facts);
    printf("Initial value: %u\n", init_value);

    return OK;
}