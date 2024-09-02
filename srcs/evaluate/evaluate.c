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

bool perform_operation(char operation, bool left_operand, bool right_operand)
{
    switch (operation)
    {
        case '+':
            return left_operand && right_operand;
        case '|':
            return left_operand || right_operand;
        case '^':
            return left_operand != right_operand;
        case '!':
            return !right_operand;
        default:
        /* should never happen, could even assert. */
            return false;
    }
}

bool evaluate_rule(t_rule *rule, uint *value)
{
    t_token *current_token = rule->facts;

    bool operand_stack[STACK_SIZE];
    char operator_stack[STACK_SIZE];
    int operand_top = -1;
    int operator_top = -1;

    while (current_token)
    {
        if (current_token->type == LETTER)
        {
            bool is_set = (*value & current_token->value) != 0;

            operand_stack[++operand_top] = is_set;
        }
        else if (current_token->type == OPERATOR)
        {
            char operator = (char)current_token->value;

            if (operator == '(')
            {
                operator_stack[++operator_top] = operator;
            }
            else if (operator == ')')
            {
                while (operator_stack[operator_top] != '(')
                {
                    bool right_operand = operand_stack[operand_top--];
                    bool left_operand = operand_stack[operand_top--];
                    char operation = operator_stack[operator_top--];
                    operand_stack[++operand_top] = perform_operation(operation, left_operand, right_operand);
                }
                operator_top--;
            }
            else
            {
                while (operator_top >= 0 && operator_stack[operator_top] != '(')
                {
                    char operation = operator_stack[operator_top--];
                    bool right_operand = operand_stack[operand_top--];
                    bool left_operand = operand_stack[operand_top--];
                    operand_stack[++operand_top] = perform_operation(operation, left_operand, right_operand);
                }
                operator_stack[++operator_top] = operator;
            }
        }

        current_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
    }

    while (operator_top >= 0)
    {
        char operation = operator_stack[operator_top--];
        bool right_operand = operand_stack[operand_top--];
        bool left_operand = operand_stack[operand_top--];
        operand_stack[++operand_top] = perform_operation(operation, left_operand, right_operand);
    }

    if (operand_top > 0)
    {
        fprintf(stderr, "Stack size is: %d.\n", operand_top);
        ft_assert(operand_top == 0, "Invalid rule. Stack size is not 1.");
    }

    bool final_result = operand_stack[operand_top];

    if (final_result)
    {
        *value |= rule->conclusion->value;
    }

    return final_result;
}

int process_rules(t_rule *rules, uint* value)
{
    t_rule *current_rule = rules;
    t_token *current_token;

    for (int i = 0; i < 5; i++)
    {
        current_rule = rules;
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

void print_marked_letters(uint value)
{
    const char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const uint masks[] = {A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};

    printf("True letters: ");
    for (int i = 0; i < 26; i++)
    {
        if (value & masks[i])
        {
            printf("%c ", letters[i]);
        }
    }
    printf("\n");
}

void process_queries(t_rule *queries, uint value)
{
    t_rule *current_query = queries;
    t_token *current_token;

    while (current_query)
    {
        current_token = current_query->facts;
        while (current_token)
        {
            if (current_token->type == LETTER)
            {
                bool is_set = (value & current_token->value) != 0;
                printf("%c is %s\n", 'A' + __builtin_ctz(current_token->value), is_set ? "true" : "false");
            }
            current_token = FT_LIST_GET_NEXT(&current_query->facts, current_token);
        }
        current_query = FT_LIST_GET_NEXT(&queries, current_query);
    }
}

int evaluate(t_expert_system *rules)
{
    uint init_value = get_initial_value(rules->initial_values->facts);

    process_rules(rules->rules, &init_value);

    print_marked_letters(init_value);

    process_queries(rules->queries, init_value);

    return OK;
}
