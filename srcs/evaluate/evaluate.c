#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>
#include <stdio.h>
#include <parser.h>
#include <stdlib.h>
#include <time.h>
#include <interactive.h>

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

static void evaluate_conclusion(t_token *conclusion, uint *value)
{
    t_token *current_token = conclusion;
    char operator_stack[STACK_SIZE];
    int operator_top = -1;
    int prev_letter = -1;

    while (current_token)
    {
        if (current_token->type == OPERATOR && current_token->value == '!')
        {
            // Directly handle negation
            current_token = FT_LIST_GET_NEXT(&conclusion, current_token);
            if (current_token && current_token->type == LETTER)
            {
                // printf("Negating %c\r\n", 'A' + __builtin_ctz(current_token->value));
                *value &= ~current_token->value;  // Turn off the bit corresponding to the letter
                prev_letter = current_token->value;
            }
        }
        else if (current_token->type == LETTER)
        {
            // Handle OR logic to ensure only one is set to true
            // Handle OR logic to ensure only one is set to true
            if (operator_top >= 0 && operator_stack[operator_top] == '|')
            {
                if (prev_letter != 0)  // There was a previous letter
                {
                    // Randomly pick between the previous letter and the current one
                    if (rand() % 2 != 0)
                    {
                        // Pick the current letter, negate the previous one
                        *value |= current_token->value;
                        *value &= ~prev_letter;
                    }
                    else
                    {
                        // Pick the previous letter, negate the current one
                        *value |= prev_letter;
                        *value &= ~current_token->value;
                    }
                    // Reset the stack after OR operation
                    operator_top--;
                }
            }
            else
            {
                *value |= current_token->value;
                prev_letter = current_token->value;
                // printf("Setting %c to true\r\n", 'A' + __builtin_ctz(current_token->value));
            }
        }
        else if (current_token->type == OPERATOR)
        {
            if ((char)current_token->value == '|' || (char)current_token->value == '^')
            {
                operator_stack[++operator_top] = '|';
            }
        }

        current_token = FT_LIST_GET_NEXT(&conclusion, current_token);
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
        if (current_token->type == OPERATOR && current_token->value == '!')
        {
            // Directly handle negation
            current_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
            if (current_token && current_token->type == LETTER)
            {
                // printf("Negating %c\r\n", 'A' + __builtin_ctz(current_token->value));
                operand_stack[++operand_top] = !(*value & current_token->value);
            }
            else
            {
                fprintf(stderr, "Invalid rule. Negation must be followed by a letter.\r\n");
                ft_assert(false, "Invalid rule. Negation must be followed by a letter.");
            }
        }
        else if (current_token->type == LETTER)
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
        fprintf(stderr, "Stack size is: %d.\r\n", operand_top);
        ft_assert(operand_top == 0, "Invalid rule. Stack size is not 1.");
    }

    bool final_result = operand_top >= 0 ? operand_stack[operand_top] : false;

    if (final_result)
    {
        evaluate_conclusion(rule->conclusion, value);
    }

    return final_result;
}

void print_marked_letters(uint value)
{
    const char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const uint masks[] = {A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};

    for (int i = 0; i < 26; i++)
    {
        if (value & masks[i])
        {
            printf("%c ", letters[i]);
        }
    }
}

int process_rules(t_rule *rules, uint *value, uint *queries_value, bool is_verbose)
{
    t_rule *current_rule = rules;
    uint prev_value;

    for (int i = 0; i < 5; i++)
    {
        current_rule = rules;
        while (current_rule)
        {
            prev_value = *value;

            evaluate_rule(current_rule, value);

            if (is_verbose)
            {
                uint diff = *value ^ prev_value;

                uint queries_diff = diff & *queries_value;

                if (queries_diff)
                {
                    printf("Value ");

                    for (int i = 0; i < 26; i++)                {
                        uint mask = 1 << i;
                        if (queries_diff & mask)
                        {
                            bool is_true = (*value & mask) != 0;
                            printf("%c changed to %s ", 'A' + i, is_true ? "true" : "false");
                        }
                    }

                    printf("due to rule: '");
                    
                    print_tokens(current_rule->facts);
                    printf(" => ");
                    print_tokens(current_rule->conclusion);
                    printf("'\r\n");
                }
            }
            current_rule = FT_LIST_GET_NEXT(&rules, current_rule);
        }
    }

    return OK;
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
                printf("%c is %s\r\n", 'A' + __builtin_ctz(current_token->value), is_set ? "true" : "false");
            }
            current_token = FT_LIST_GET_NEXT(&current_query->facts, current_token);
        }
        current_query = FT_LIST_GET_NEXT(&queries, current_query);
    }
}

int evaluate(t_expert_system *rules, bool is_verbose)
{
    uint init_value = get_initial_value(rules->initial_values->facts);
    uint queries_value = get_initial_value(rules->queries->facts);


    process_rules(rules->rules, &init_value, &queries_value, is_verbose);

    printf("Letters evaluated as true: ");
    print_marked_letters(init_value);
    printf("\r\n");

    process_queries(rules->queries, init_value);

    return OK;
}
