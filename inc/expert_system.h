#ifndef EXPERT_SYSTEM_H
#define EXPERT_SYSTEM_H
#include <ft_list.h>

#define uint unsigned int

typedef enum
{
    false,
    true
} bool;


typedef enum
{
    LETTER,
    OPERATOR,
} t_token_type;

typedef struct s_token
{
    list_item_t l;
    t_token_type type;
    uint value;
} t_token;

typedef struct s_rule {
    list_item_t l;
    t_token *facts;
    t_token *conclusion;
} t_rule;

typedef struct s_expert_system
{
    struct s_rule *rules;
    struct s_rule *initial_values;
    struct s_rule *queries;
} t_expert_system;


#define A 0x00000001
#define B 0x00000002
#define C 0x00000004
#define D 0x00000008
#define E 0x00000010
#define F 0x00000020
#define G 0x00000040
#define H 0x00000080
#define I 0x00000100
#define J 0x00000200
#define K 0x00000400
#define L 0x00000800
#define M 0x00001000
#define N 0x00002000
#define O 0x00004000
#define P 0x00008000
#define Q 0x00010000
#define R 0x00020000
#define S 0x00040000
#define T 0x00080000
#define U 0x00100000
#define V 0x00200000
#define W 0x00400000
#define X 0x00800000
#define Y 0x01000000
#define Z 0x02000000



#endif
