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
bool evaluate_rule(t_rule *rule, uint *value) {
    t_token *current_token = rule->facts;
    bool result = false;
    // bool temp = false;  // Used for intermediate operations

    char last_operator = '\0';  // Stores the last operator encountered

    while (current_token) {
        if (current_token->type == LETTER) {
            // Evaluate the current letter by checking if its bit is set in *value
            bool is_set = (*value & current_token->value) != 0;

            if (last_operator == '\0') {
                // First letter, initialize the result
                result = is_set;
            } else if (last_operator == '+') {
                // AND operation
                result = result && is_set;
            } else if (last_operator == '|') {
                // OR operation
                result = result || is_set;
            } else if (last_operator == '^') {
                // XOR operation
                result = result != is_set;
            } else if (last_operator == '!') {
                // NOT operation (applies to the next token)
                result = !is_set;
            }
        } else if (current_token->type == OPERATOR) {
            // Update the last operator
            last_operator = (char)current_token->value;
        }

        // Move to the next token
        current_token = FT_LIST_GET_NEXT(&rule->facts, current_token);
    }

    return result;
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
    uint masks[] = {A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z};

    printf("Marked letters: ");
    for (int i = 0; i < 26; i++)
    {
        if (value & masks[i])
        {
            printf("%c ", letters[i]);
        }
    }
    printf("\n");
}

int evaluate(t_expert_system *rules)
{
    printf("Evaluating rules...\n");

    uint init_value = get_initial_value(rules->initial_values->facts);
    printf("Initial value: %u\n", init_value);
    process_rules(rules->rules, &init_value);
    printf("Final value: %u\n", init_value);
    print_marked_letters(init_value);
    

    return OK;
}