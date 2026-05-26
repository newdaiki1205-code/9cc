#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

typedef enum {
    ADD,
    MULTI,
    VAL
} t_type;


typedef struct node {
    t_type type;
    int val;
    struct node *l;
    struct node *r;
}   node;

node    *parse_expr(char **s, int flag);
void    destroy_tree(node *n);



void    destroy_tree(node *n)
{
    if (!n)
        return ;
    if (n->type != VAL)
    {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}

void    unexpected(char c)
{
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of input\n");
}

int accept(char **s, char c)
{
    if (**s && **s == c)
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c)
{
    if (accept(s, c))
        return (1);
    unexpected(**s);
    return (0);
}


int eval_tree(node *tree) //chech if its NULL
{
    if(!tree)
        return 0;
    
    switch (tree->type)
    {
        case ADD:
            return (eval_tree(tree->l) + eval_tree(tree->r));
        case MULTI:
            return (eval_tree(tree->l) * eval_tree(tree->r));
        case VAL:
            return (tree->val);
    }
}



node    *new_node(t_type type, node *lhs, node *rhs)
{
    node *ret = calloc(1, sizeof(node));
    if (!ret)
        return (NULL);
    ret->l = lhs;
    ret->r = rhs;
    ret->type = type;
    if(!ret->r)
        return (destroy_tree(ret), NULL);
    return (ret);
}

node *new_node_num(int val)
{
    node *ret = calloc(1, sizeof(node *));
    ret->type = VAL;
    ret->val = val;
    return ret;
}

node *primary(char **s)
{
    node *ret = NULL;;

    if(isdigit(**s))
    {
        ret = new_node_num(**s - '0');
        if(!accept(s, **s))
            return (NULL);
    }
    else if(expect(s, '('))
    {
        ret = parse_expr(s, 1);
        if(!expect(s, ')'))
            return (destroy_tree(ret), NULL);
    }
    else
        return NULL;
    return ret;
}

node    *mul(char **s)
{
    node *ret = primary(s);
    if(!ret)
        return NULL;

    while(1)
    {
        if(**s && accept(s, '*'))
            ret = new_node(MULTI, ret, primary(s));
        else
            break;
    }
    return ret;
}

node    *parse_expr(char **s, int flag)
{
    node *ret;

    ret = mul(s);
    if(!ret)
        return (NULL);
    while(1)
    {
        if(**s && accept(s, '+'))
            ret = new_node(ADD, ret, mul(s));
        else
            break;
    }
    if(**s && !flag)
        return (unexpected(**s), destroy_tree(ret), NULL);
    return (ret);
}


int main(int argc, char **argv)
{
    if (argc != 2)
        return (1);
    node *tree = parse_expr(&argv[1], 0);
    if (!tree)
        return (1);
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
}