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

/* this is not valid */
bool evaluate_rule(t_rule *rule, uint* value)
{
    t_token *current_token = rule->facts;
    uint rule_value = 0;

    while (current_token)
    {
        if (current_token->type == LETTER)
        {
            rule_value |= current_token->value;
        }
        current_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
    }

    return (*value & rule_value) == rule_value;
}

int process_rules(t_rule *rules, uint* value)
{
    t_rule *current_rule = rules;
    t_token *current_token;
    

    for (int i = 0; i < 5; i++)
    {
        while (current_rule)
        {
            if (evaluate_rule(current_rule, value))
            {
                current_token = current_rule->conclusion;
                while (current_token)
                {
                    if (current_token->type == LETTER)
                    {
                        *value |= current_token->value;
                    }
                    current_token = FT_LIST_GET_NEXT(&current_rule->conclusion, current_token);
                }
            }
            current_rule = FT_LIST_GET_NEXT(&rules, current_rule);
        }
    }

    return OK;
}

int evaluate(t_expert_system *rules)
{
    printf("Evaluating rules...\n");

    uint init_value = get_initial_value(rules->initial_values->facts);
    printf("Initial value: %u\n", init_value);

    return OK;
}