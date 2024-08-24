#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>
#include <stdio.h>
#include <parser.h>

/*
    initial value it's supposed to be allways at 
    2nd position starting from the end
*/
uint get_initial_value(t_rule *rules)
{
    t_rule *current_rule = rules;
    uint value = 0;

    while (current_rule)
    {
        if (!current_rule->facts)
        {
            current_rule = FT_LIST_GET_PREV(&rules, current_rule);
            continue;
        }

        value = get_token_value(current_rule->facts);
        if (value == '=')
        {
            printf("Initial value found: %c\n", value);
            break;
        }

        current_rule = FT_LIST_GET_PREV(&rules, current_rule);
    }

    t_token *current_token = current_rule->facts;
    value = 0;
    while (current_token)
    {
        if (current_token->type == LETTER)
        {
            value |= current_token->value;
        }

        current_token = FT_LIST_GET_NEXT(&current_rule->facts, current_token);
    }

    return value;

}

bool perform_operation(uint operation, uint right_operand, uint left_operand, uint value)
{
    printf("Performing operation %d with operand %d\n", operation, right_operand);
    switch (operation)
    {
        case '+':
            if (value & right_operand && value & left_operand)
            {
                return true;
            }
            break;
        case '|':
            if (value & right_operand || value & left_operand)
            {
                return true;
            }
            break;
        case '^':
            if ((value & right_operand) ^ (value & left_operand))
            {
                return true;
            }
            break;
        case '!':
            if (value & right_operand)
            {
                return true;
            }
            break;
        default:
            fprintf(stderr, "Invalid operation %c\n", operation);
            ft_assert(operation == '+' || operation == '|' || operation == '^' || operation == '!', "Invalid operation");
    }
    return false;
}

void evaluate_rule(uint *value, t_rule *rule)
{
    t_token *current_token = rule->facts;
    t_token *aux_token = NULL;
    bool first_operand = true;
    uint left_operand = 0;
    uint right_operand = 0;
    uint operation = 0;
    bool result = false;

    /* size 1 means only one letter, then we check it directly. */
    if (FT_LIST_GET_SIZE(&rule->facts) == 1)
    {
        if (*value & rule->facts->value)
        {
            *value |= rule->conclusion->value;
        }
        else
        {
            /* NOTHING */
        }
        return;
    }

    while (current_token)
    {
        if (current_token->type == OPERATOR)
        {
            if (first_operand)
            {
                aux_token = FT_LIST_GET_PREV(&rule->facts, current_token);
                if (!aux_token)
                {
                    aux_token = rule->facts;
                }
                left_operand = get_token_value(aux_token);
            }
            aux_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
            right_operand = aux_token->value;
            operation = current_token->value;
            result = perform_operation(operation, right_operand, left_operand, *value);
        }

        current_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
    }

    if (result)
    {
        *value |= rule->conclusion->value;
    }
}

int perform_evaluation(uint *value, t_rule *rule)
{
    t_rule *current_rule = rule;

    for (int i = 0; i < 2; i++)
    {
        while (current_rule)
        {
            evaluate_rule(value, current_rule);


            current_rule = FT_LIST_GET_NEXT(&current_rule, current_rule);
        }

        current_rule = FT_LIST_GET_NEXT(&rule, current_rule);
    }


    return OK;
}


int evaluate(t_rule *rules)
{
    printf("Evaluating rules...\n");

    uint initial_value = get_initial_value(rules);
    // t_rule *current_rule = rules;

    printf("Initial value: %d\n", initial_value);

    perform_evaluation(&initial_value, rules);

    printf("Final value: %d\n", initial_value);

    // check_result(rules, initial_value);


    return OK;
}