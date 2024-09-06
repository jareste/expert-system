#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>

#include <expert_system.h>
#include <interactive.h>
#include <evaluate.h>
#include <parser.h>


#define INPUT_SIZE 256

static void print_menu()
{
    printf("%s", MENU_INTERACTIVE);
}

static struct termios terminal_settings;

static void modify_terminal_settings()
{
    struct termios newt;
    tcgetattr(STDIN_FILENO, &terminal_settings);
    newt = terminal_settings;

    cfmakeraw(&newt);

    newt.c_cc[VEOF] = _POSIX_VDISABLE;
    newt.c_cc[VINTR] = terminal_settings.c_cc[VINTR];
    newt.c_cc[VREPRINT] = _POSIX_VDISABLE;
    newt.c_cc[VKILL] = _POSIX_VDISABLE;
    newt.c_cc[VWERASE] = _POSIX_VDISABLE;
    newt.c_cc[VSUSP] = _POSIX_VDISABLE;

    newt.c_lflag &= ~ECHOCTL;

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

static void restore_terminal_settings()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &terminal_settings);
}

static char read_stdin_char()
{
    char ch;

    printf(">> ");
    fflush(stdout);

    ch = getchar();
    
    putchar(ch);
    fflush(stdout);
    printf("\r\n");

    if (isalpha(ch))
    {
        return ch;
    }
    return read_stdin_char();
}

void modify_initial_values(t_rule* initial_values, bool is_query)
{
    printf("Modify %s: (Enter uppercase letters A-Z). Pres enter to exit.\r\n", is_query ? "queries" : "initial values");

    free_rules(initial_values);

    uint new_initial_values = 0;
    char input;
    
    bool letters_used[26] = {false};

    while ((input = getchar()) != '\r')
    {
        input = toupper(input);
        if (input >= 'A' && input <= 'Z')
        {
            int index = input - 'A';
            if (!letters_used[index])
            {
                letters_used[index] = true;
                new_initial_values |= (1 << index);
                printf("Added %c to %s\r\n", input, is_query ? "queries" : "initial values");
            }
            else
            {
                printf("Letter %c is already used. Please enter a different letter.\r\n", input);
            }
        }
        else
        {
            printf("Invalid input. Please enter uppercase letters A-Z only.\r\n");
        }
    }

    t_rule *new_rule = malloc(sizeof(t_rule));
    t_token *new_tokens = NULL;

    for (int i = 0; i < 26; i++)
    {
        if (new_initial_values & (1 << i))
        {
            t_token *token = malloc(sizeof(t_token));
            token->type = LETTER;
            token->value = (1 << i);
            FT_LIST_ADD_LAST(&new_tokens, token);
        }
    }

    new_rule->facts = new_tokens;
    new_rule->conclusion = NULL;

    initial_values = new_rule;

    printf("%s modified successfully.\r\n", is_query ? "Queries" : "Initial values");
}

void print_tokens(t_token *tokens)
{
    t_token *current_token = tokens;

    while (current_token)
    {
        if (current_token->type == LETTER)
        {
            printf("%c ", 'A' + __builtin_ctz(current_token->value));
        }
        else
        {
            printf("%c ", (char)current_token->value);
        }
        current_token = FT_LIST_GET_NEXT(&tokens, current_token);
    }
}

static void print_rules(t_rule *rules)
{
    t_rule *current_rule = rules;
    uint counter = 1;

    while (current_rule)
    {
        printf("%d: ", counter++);
        print_tokens(current_rule->facts);

        printf(" => ");

        print_tokens(current_rule->conclusion);

        printf("\r\n");

        current_rule = FT_LIST_GET_NEXT(&rules, current_rule);
    }
}

static int read_number_or_cancel()
{
    char input[4];
    int i = 0;
    char ch;

    printf("Enter a rule number (1-999) or 'q' to cancel: ");
    fflush(stdout);

    while ((ch = getchar()) != '\r' && i < 3)
    {
        if (ch == 'q') 
        {
            return -1;
        }

        if (isdigit(ch)) 
        {
            putchar(ch);
            input[i++] = ch;
        } 
        else 
        {
            printf("Invalid input. Please enter a number between 1 and 999 or 'q' to cancel.\r\n");
            return read_number_or_cancel();
        }
    }

    input[i] = '\0';

    return atoi(input);
}

void remove_rule_by_number(t_expert_system* es)
{
    t_rule *rules = es->rules;
    int rule_number = read_number_or_cancel();

    if (rule_number == -1) 
    {
        printf("\r\nOperation cancelled.\r\n");
        return;
    }

    t_rule *current_rule = rules;
    int current_index = 1;

    while (current_rule && current_index < rule_number) 
    {
        current_rule = FT_LIST_GET_NEXT(&rules, current_rule);
        current_index++;
    }

    if (current_rule && current_index == rule_number) 
    {
        printf("\r\nRemoving rule %d...\r\n", rule_number);

        FT_LIST_POP(&rules, current_rule);

        if (current_rule->facts)
            free_tokens(current_rule->facts);
        if (current_rule->conclusion)
            free_tokens(current_rule->conclusion);
        
        free(current_rule);
        printf("\r\nRule %d removed successfully.\r\n", rule_number);
    } 
    else 
    {
        printf("\r\nInvalid rule number. No rule found at position %d.\r\n", rule_number);
    }
    es->rules = rules;
}


t_token* parse_side(const char* input)
{
    t_token *tokens = NULL;

    char *token_str = strtok(strdup(input), "+");
    while (token_str != NULL)
    {
        char letter = toupper(token_str[0]);
        if (letter >= 'A' && letter <= 'Z')
        {
            t_token *new_token = malloc(sizeof(t_token));
            new_token->type = LETTER;
            new_token->value = (1 << (letter - 'A'));
            FT_LIST_ADD_LAST(&tokens, new_token);
        }
        token_str = strtok(NULL, "+");
    }

    return tokens;
}

void add_new_rule(t_expert_system* es)
{
    char facts[INPUT_SIZE] = {0};
    char conclusion[INPUT_SIZE] = {0};
    int fact_index = 0, conclusion_index = 0;
    bool reading_facts = true;
    char ch;

    printf("Enter the new rule in format (facts => conclusion): ");
    fflush(stdout);

    while ((ch = getchar()) != '\r')
    {
        ch = toupper(ch);
        if (reading_facts)
        {
            if (ch == '=')
            {
                reading_facts = false;
                printf(" => ");
            }
            else if (isalpha(ch) || ch == '+' || ch == '|' || ch == '^' || ch == '(' || ch == ')' || '!')
            {
                facts[fact_index++] = ch;
                putchar(ch);
            }
            else
            {
                printf("\r\nInvalid character in facts. Use only uppercase letters, '+', '|', '^', '!', '(', and ')'.\r\n");
                return;
            }
        }
        else
        {
            if (isalpha(ch) || ch == '+' || ch == '|' || ch == '^' || ch == '(' || ch == ')' || ch == '!')
            {
                conclusion[conclusion_index++] = ch;
                putchar(ch);
            }
            else
            {
                printf("\r\nInvalid character in conclusion. Use only uppercase letters, '+', '|', '^', '!', '(', and ')'.\r\n");
                return;
            }
        }
    }

    printf("\r\n");

    char whole_line[INPUT_SIZE * 2 + 3] = {0};
    snprintf(whole_line, sizeof(whole_line), "%s=>%s", facts, conclusion);

    t_rule *new_rule = process_line(whole_line);
    
    FT_LIST_ADD_LAST(&es->rules, new_rule);

    printf("New rule added successfully: %s => %s\r\n", facts, conclusion);
}



int interactive_mode(t_expert_system* es, bool is_verbose)
{
    uint initial_values = 0;
    modify_terminal_settings();

    print_menu();
    (void)es;
    do
    {
        char ch = read_stdin_char();
        switch (ch) 
        {
            case 'Q':
            case 'q':
                restore_terminal_settings();
                return 0;
            case 'S':
            case 's':
                print_rules(es->rules);
                break;
            case 'I':
            case 'i':
                initial_values = get_initial_value(es->initial_values->facts);
                printf("Initial values:\r\n");
                print_marked_letters(initial_values);
                printf("\r\n");
                initial_values = 0;
                break;
            case 'P':
            case 'p':
                initial_values = get_initial_value(es->queries->facts);
                printf("Queries:\r\n");
                print_marked_letters(initial_values);
                printf("\r\n");
                initial_values = 0;
                break;
            case 'R':
            case 'r':
                remove_rule_by_number(es);
                break;
            case 'N':
            case 'n':
                add_new_rule(es);
                break;
            case 'M':
            case 'm':
                modify_initial_values(es->initial_values, false);
                break;
            case 'K':
            case 'k':
                modify_initial_values(es->queries, true);
                break;
            case 'H':
            case 'h':
                print_menu();
                break;
            case 'E':
            case 'e':
                evaluate(es, is_verbose);
                break;
            case 'V':
            case 'v':
                is_verbose = !is_verbose;
                printf("Verbose mode %s.\r\n", is_verbose ? "enabled" : "disabled");
                break;
            default:
                printf("Unknown command\r\n");
                break;
        }
        printf("\r\n");


    } while (1);


    restore_terminal_settings();

    return 0;
}