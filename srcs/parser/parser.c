#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

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

int check_balanced_parentheses(const char *line)
{
    int balance = 0;
    while (*line)
    {
        if (*line == '(')
        {
            balance++;
        }
        else if (*line == ')')
        {
            balance--;
            if (balance < 0)
            {
                return 0;
            }
        }
        line++;
    }
    return balance == 0;
}

int check_uppercase_in_rightside(const char *line)
{
    while (*line)
    {
        if (isupper(*line))
        {
            return 1;
        }
        line++;
    }
    return 0;
}

char* trim_whitespace(char *str)
{
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = 0;

    return str;
}

int check_file_format(FILE *file)
{
    char line[1024];
    int has_equal_line = 0;
    int has_question_line = 0;

    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\r\n")] = '\0';

        char *comment_pos = strchr(line, '#');
        if (comment_pos)
        {
            *comment_pos = '\0';
        }

        char *trimmed_line = trim_whitespace(line);

        if (strlen(trimmed_line) == 0)
        {
            continue;
        }

        if (trimmed_line[0] == '=')
        {
            has_equal_line = 1;
            continue;
        }

        if (trimmed_line[0] == '?')
        {
            has_question_line = 1;
            continue;
        }

        char *arrow_pos = strstr(trimmed_line, "=>");
        if (arrow_pos == NULL)
        {
            fprintf(stderr, "No arrow found in line: %s\r\n", trimmed_line);
            ft_assert(0, "Fatal error: No arrow found in line.");
            return 0;
        }

        *arrow_pos = '\0';
        const char *left_side = trimmed_line;
        const char *right_side = arrow_pos + 2;

        for (const char *c = left_side; *c; c++)
        {
            if (!isalnum(*c) && *c != '|' && *c != '+' && *c != '^' && *c != '(' && *c != ')' && *c != '!' && *c != ' ' && *c != '<')
            {
                fprintf(stderr, "Invalid character: %c\r\n", *c);
                ft_assert(0, "Fatal error: Invalid character.");
                return 0;
            }
        }

        if (!check_balanced_parentheses(left_side))
        {
            fprintf(stderr, "Unbalanced parentheses.\r\n");
            ft_assert(0, "Fatal error: Unbalanced parentheses.");
            return 0;
        }

        if (!check_uppercase_in_rightside(right_side))
        {
            fprintf(stderr, "Right side must contain at least one uppercase letter.\r\n");
            ft_assert(0, "Fatal error: Right side must contain at least one uppercase letter.");
            return 0;
        }
    }

    return has_equal_line && has_question_line;
}

static void read_file(const char *filename, char **content)
{
    if (access(filename, F_OK) != 0)
    {
        fprintf(stderr, "expert-system: %s: No such file or directory\r\n", filename);
        ft_assert(access(filename, F_OK) == 0, "Fatal error: File does not exist.");
    }

    if (access(filename, R_OK) != 0)
    {
        fprintf(stderr, "expert-system: %s: Permission denied\r\n", filename);
        ft_assert(access(filename, R_OK) == 0, "Fatal error: Permission denied.");
    }

    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        fprintf(stderr, "expert-system: %s: %s\r\n", filename, strerror(errno));
        /* NEVER HERE */
        ft_assert(file, "Fatal error: Could not open file.");
    }
    
    if (!check_file_format(file)) {
        fprintf(stderr, "expert-system: %s: Invalid file format\r\n", filename);
        ft_assert(0, "Fatal error: Invalid file format.");
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

t_rule* process_line(char *line)
{
    int i = 0;
    t_rule *rule = malloc(sizeof(t_rule));
    t_token *facts = NULL;
    t_token *conclusion = NULL;
    t_token *n_token = NULL;
    bool equal_found = false;
    bool biconditional_found = false;

    if (line[0] == '?')
        return NULL;

    for (i = 0; line[i]; i++)
    {
        switch (line[i])
        {
            case '<':
                biconditional_found = true;
            case ' ':
            case '>':
                continue;
            case '?':
                fprintf(stderr, "Invalid character in rule %c.\r\n", line[i]);
                ft_assert(i == -1, "Invalid character in rule");
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
                if (equal_found)
                {
                    fprintf(stderr, "Found more than one equal in rule: %s.\r\n", line);
                    ft_assert(equal_found == false, "More than one equal found in rule.");
                }
                equal_found = true;
                continue;
            case '(':
                n_token = create_token(OPERATOR, '(');
                break;
            case ')':
                n_token = create_token(OPERATOR, ')');
                break;
            case '#':
                break;
            default:
                if (line[i] >= 'A' && line[i] <= 'Z')
                {
                    n_token = create_token(LETTER, 1 << (line[i] - 'A'));
                }
                else
                {
                    fprintf(stderr, "Invalid character in rule %c.\r\n", line[i]);
                    ft_assert(line[i] >= 'A' && line[i] <= 'Z', "Invalid character in rule");
                }
                break;
        }
        
        if (line[i] == '#')
        {
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
    if (biconditional_found && rand() % 2 == 0)
    {
        rule->facts = conclusion;
        rule->conclusion = facts;
        
    }
    else
    {
        rule->facts = facts;
        rule->conclusion = conclusion;
    }

    return rule;
}


static t_rule* process_content(char *content)
{
    t_rule *rules = NULL;
    t_rule *n_rule = NULL;
    
    char *line = strtok(content, "\r\n");
    while (line)
    {
        line += strspn(line, " \t");

        if (line[0] == '=' || line[0] == '?')
        {
            line = strtok(NULL, "\r\n");
            continue;
        }

        n_rule = process_line(line);
        if (!n_rule)
        {
            break;
        }

        if (n_rule->facts == NULL && n_rule->conclusion == NULL)
        {
            free(n_rule);
            break;
        }

        FT_LIST_ADD_LAST(&rules, n_rule);
  
        line = strtok(NULL, "\r\n");
    }
    return rules;
}

static t_rule* process_initial_values(char *content)
{
    t_rule *initial_values = NULL;
    t_rule *n_rule = NULL;
    regex_t regex;
    regmatch_t pmatch[1];
    const char *pattern = "\n[ \t]*=";

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        printf("Could not compile regex\r\n");
        return NULL;
    }

    if (regexec(&regex, content, 1, &pmatch[0], 0) == 0)
    {
        char *line = content + pmatch[0].rm_eo;
        char *line_end = strchr(line, '\n');
        if (line_end)
        {
            *line_end = '\0';
        }

        for (int i = 0; line[i]; i++)
        {
            if (line[i] == '#')
            {
                break;
            }
            if (line[i] < 'A' || line[i] > 'Z')
            {
                fprintf(stderr, "Invalid character in initial values %c.\r\n", line[i]);
                ft_assert(line[i] >= 'A' && line[i] <= 'Z', "Invalid character in initial values");
            }
        }

        n_rule = process_line(line);
        if (!n_rule)
        {
            printf("No initial values found\r\n");
        }
        else
        {
            FT_LIST_ADD_LAST(&initial_values, n_rule);
        }
        
        if (line_end) {
            *line_end = '\n';
            if (regexec(&regex, line_end, 1, &pmatch[0], 0) == 0)
            {
                ft_assert(0, "Fatal error: More than one initial values found.");
            }
        }
    }
    else
    {
        fprintf(stderr, "Could not find line starting by =.\r\n");
        ft_assert(initial_values, "Fatal error: No initial values found on file.");
    }

    regfree(&regex);

    return initial_values;
}

static t_rule* process_queries(char *content)
{
    t_rule *queries = NULL;
    t_rule *n_rule = NULL;
    regex_t regex;
    regmatch_t pmatch[1];
    const char *pattern = "\n[ \t]*\\?";

    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Fatal error: could not compile regex\r\n");
        ft_assert(0, "Fatal error: could not compile regex");
    }

    if (regexec(&regex, content, 1, &pmatch[0], 0) == 0)
    {
        char *line = content + pmatch[0].rm_eo;
        char *line_end = strchr(line, '\n');
        if (line_end)
        {
            *line_end = '\0';
        }

        for (int i = 0; line[i]; i++)
        {
            if (line[i] == '#')
            {
                break;
            }
            if (line[i] < 'A' || line[i] > 'Z')
            {
                fprintf(stderr, "Invalid character in queries %c.\r\n", line[i]);
                ft_assert(line[i] >= 'A' && line[i] <= 'Z', "Invalid character in queries");
            }
        }

        n_rule = process_line(line);
        if (!n_rule)
        {
            printf("No queries found\r\n");
        }
        else
        {
            FT_LIST_ADD_LAST(&queries, n_rule);
        }
        
        if (line_end)
        {
            *line_end = '\n';
            if (regexec(&regex, line_end, 1, &pmatch[0], 0) == 0)
            {
                ft_assert(0, "Fatal error: More than one queries values found.");
            }
        }
    }
    else
    {
        fprintf(stderr, "Could not find line starting by ?.\r\n");
        ft_assert(queries, "Fatal error: No queries found on file.");
    }

    regfree(&regex);

    return queries;
}

int parse(char* filename, t_expert_system *es)
{
    char *content = NULL;
    read_file(filename, &content);

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

