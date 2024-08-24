#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <ft_malloc.h>
#include <ft_list.h>
#include <expert_system.h> 
#include <error_codes.h>

uint get_token_value(t_token *token)
{
    return token->value;
}

static void free_tokens(t_token *tokens)
{
    if (!tokens) return;

    t_token *current_token = tokens;
    t_token *next_token;

    while (current_token)
    {
        next_token = FT_LIST_GET_NEXT(&tokens, current_token);
        FT_LIST_POP(&tokens, current_token);
        free(current_token);

        /**/
        current_token = next_token;
    }
}

void free_rules(t_rule *rules)
{
    if (!rules) return;

    t_rule *current_rule = rules;
    t_rule *next_rule;

    while (current_rule)
    {
        next_rule = FT_LIST_GET_NEXT(&rules, current_rule);
        free_tokens(current_rule->facts);
        free_tokens(current_rule->conclusion);
        FT_LIST_POP(&rules, current_rule);
        free(current_rule);

        /**/
        current_rule = next_rule;
    }
}

static void read_file(const char *filename, char **content)
{
    if (access(filename, F_OK) != 0)
    {
        fprintf(stderr, "ft_ssl: %s: No such file or directory\n", filename);
        ft_assert(access(filename, F_OK) == 0, "Fatal error: File does not exist.");
    }

    if (access(filename, R_OK) != 0)
    {
        fprintf(stderr, "ft_ssl: %s: Permission denied\n", filename);
        ft_assert(access(filename, R_OK) == 0, "Fatal error: Permission denied.");
    }

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "ft_ssl: %s: %s\n", filename, strerror(errno));
        /* NEVER HERE */
        ft_assert(file, "Fatal error: Could not open file.");
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *content = malloc(file_size + 1);

    size_t read_size = fread(*content, 1, file_size, file);
    if (read_size != (size_t)file_size)
    {
        perror("Error reading file");
        free(*content);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    (*content)[file_size] = '\0';

    fclose(file);
}

static t_token* create_token(t_token_type type, uint value)
{
    t_token *token = malloc(sizeof(t_token));
    token->type = type;
    token->value = value;
    return token;
}

static t_rule* process_line(char *line)
{
    int i = 0;
    t_rule *rule = malloc(sizeof(t_rule));
    t_token *facts = NULL;
    t_token *conclusion = NULL;
    t_token *n_token = NULL;
    bool equal_found = false;

    if (line[0] == '?')
        return NULL;

    for (i = 0; line[i]; i++)
    {
        switch (line[i])
        {
            case ' ':
            case '>':
                continue;
            case '?':
                fprintf(stderr, "Invalid character in rule %c.\n", line[i]);
                ft_assert(i == 0, "Invalid character in rule");
                break;
            case '+':
                n_token = create_token(OPERATOR, '+');
                break;
            case '|':
                n_token = create_token(OPERATOR, '|');

                break;
            case '^':
                n_token = create_token(OPERATOR, '^');
                break;
            case '!':
                n_token = create_token(OPERATOR, '!');
                break;
            case '=':
                if (i == 0)
                {
                    n_token = create_token(OPERATOR, '=');
                    break;
                }
                equal_found = true;
                continue;
            default:
                if (line[i] >= 'A' && line[i] <= 'Z')
                {
                    n_token = create_token(LETTER, 1 << (line[i] - 'A'));
                }
                else
                {
                    fprintf(stderr, "Invalid character in rule %c.\n", line[i]);
                    ft_assert(line[i] >= 'A' && line[i] <= 'Z', "Invalid character in rule");
                }
                break;
        }

        switch (equal_found)
        {
            case false:
                FT_LIST_ADD_LAST(&facts, n_token);
                break;
            case true:
                FT_LIST_ADD_LAST(&conclusion, n_token);
                break;
        }

    }
    rule->facts = facts;
    rule->conclusion = conclusion;
    return rule;
}


static t_rule* process_content(char *content)
{
    t_rule *rules = NULL;
    t_rule *n_rule = NULL;
    
    char *line = strtok(content, "\n");
    while (line)
    {
        printf("%s\n", line);
        n_rule = process_line(line);
        if (!n_rule)
        {
            break;
        }

        FT_LIST_ADD_LAST(&rules, n_rule);
  
        line = strtok(NULL, "\n");
    }
    return rules;
}


static t_rule* process_initial_values(char *content)
{
    t_rule *initial_values = NULL;
    t_rule *n_rule = NULL;
    
    printf("Processing initial values\n");
    char *line = strstr(content, "\n=");
    if (line)
    {
        line += 2; // Move past "\n="
        char *line_end = strchr(line, '\n');
        if (line_end)
        {
            *line_end = '\0';
        }
     
        printf("LINE::::::::::::::::::::::%s\n", line);
        n_rule = process_line(line);
        if (!n_rule)
        {
            printf("No initial values found\n");
        }
        else
        {
            FT_LIST_ADD_LAST(&initial_values, n_rule);
        }
        if (line_end)
        {
            *line_end = '\n';
        }
    }
    return initial_values;
}

static t_rule* process_queries(char *content)
{
    t_rule *queries = NULL;
    t_rule *n_rule = NULL;
    
    printf("Processing queries\n");
    char *line = strstr(content, "\n?");
    if (line)
    {
        line += 2; // Move past "\n?"
        char *line_end = strchr(line, '\n');
        if (line_end)
        {
            *line_end = '\0'; // Temporarily terminate the string at the newline
        }
        printf("LINE::::::::::::::::::::::%s\n", line);
        n_rule = process_line(line);
        if (!n_rule)
        {
            printf("No queries found\n");
        }
        else
        {
            FT_LIST_ADD_LAST(&queries, n_rule);
        }
        if (line_end)
        {
            *line_end = '\n'; // Restore the newline
        }
    }
    return queries;
}


int parse(char* filename, t_expert_system *es)
{
    char *content = NULL;
    read_file(filename, &content);
    printf("%s\n", content);
    printf("Parsing file CONTENT\n");
    printf("---------------------------------------\n");
    char* initial = strdup(content);
    char* queries = strdup(content);

    es->rules = process_content(content);
    es->initial_values = process_initial_values(initial);
    es->queries = process_queries(queries);

    free(content);
    free(initial);
    free(queries);
    return 0;
}

