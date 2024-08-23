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

void free_tokens(t_token *tokens)
{
    if (!tokens) return;

    t_token *current_token = tokens;
    t_token *next_token;

    while (current_token)
    {
        next_token = FT_LIST_GET_NEXT(&tokens, current_token);
        FT_LIST_POP(&tokens, current_token);
        free(current_token);
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
        free_tokens(current_rule->tokens);
        FT_LIST_POP(&rules, current_rule);
        free(current_rule);
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

t_token* create_token(t_token_type type, uint value)
{
    t_token *token = malloc(sizeof(t_token));
    token->type = type;
    token->value = value;
    return token;
}

t_rule* process_line(char *line)
{
    int i = 0;
    t_rule *rule = malloc(sizeof(t_rule));
    t_token *tokens = NULL;
    t_token *n_token = NULL;

    for (i = 0; line[i]; i++)
    {
        switch (line[i])
        {
            case ' ':
            case '>':
            case '?':
                continue;
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
                n_token = create_token(OPERATOR, '=');
                break;
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
        FT_LIST_ADD_LAST(&tokens, n_token);
    }
    rule->tokens = tokens;
    return rule;
}


t_rule* process_content(char *content)
{
    t_rule *rules = NULL;
    t_rule *n_rule = NULL;
    
    char *line = strtok(content, "\n");
    while (line)
    {
        printf("%s\n", line);
        n_rule = process_line(line);

        FT_LIST_ADD_LAST(&rules, n_rule);
  
        line = strtok(NULL, "\n");
    }
    return rules;
}


int parse(char* filename)
{
    char *content = NULL;
    read_file(filename, &content);
    printf("%s\n", content);
    printf("Parsing file CONTENT\n");
    printf("---------------------------------------\n");

    t_rule *rules = process_content(content);
    
    free_rules(rules);
    free(content);

    return 0;
}

