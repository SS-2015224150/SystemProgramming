#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

typedef struct _node
{
    ino_t ino_num ;
    struct _node* link;
}Node;

typedef struct _stack
{
    struct _node* list ;
    struct _node* cur ;
}Stack;

ino_t get_inode(char*);
void printpathto(ino_t);
void inum_to_name(ino_t , char * , int);

Stack* createStack() ;
Node* push(ino_t , Stack*);
Node* pop(Stack*);
int main()
{
    printpathto(get_inode("."));
    putchar('\n');
    return 0;
}

void printpathto(ino_t this_inode)
//    prints path leading down to an object with this inode
//    kind of recursive
{
    Stack* stack ;
    Node* curNode ;
    char its_name[BUFSIZ];
    stack = createStack();

    this_inode = get_inode(".") ;

    while(get_inode("..") != this_inode)
    {
        push(this_inode , stack);
        chdir("..");
        this_inode = get_inode(".");
    }

    for(curNode = pop(stack) ; curNode != NULL ; curNode = pop(stack))
    {
        inum_to_name(curNode->ino_num , its_name , BUFSIZ);
        printf("/%s" , its_name);
        chdir(its_name);
        free(curNode);
    }
}

void inum_to_name(ino_t inode_to_find , char *namebuf , int buflen)
{
    DIR *dir_ptr ;
    struct dirent *direntp;
    dir_ptr = opendir(".");
    if(dir_ptr == NULL)
    {
        perror(".");
        exit(1);
    }

    while((direntp = readdir(dir_ptr)) != NULL)
    {
        if(direntp->d_ino == inode_to_find)
        {
            strncpy(namebuf , direntp->d_name , buflen);
            namebuf[buflen-1] = '\0';
            closedir(dir_ptr);
            return ;
        }
    }

    fprintf(stderr , "error looking for inum %d\n" , (int)inode_to_find);
    exit(1);
}

ino_t get_inode(char* fname)
{
    struct stat info ;

    if(stat(fname , &info) == -1)
    {
        fprintf(stderr , "Cannot stat ");
        perror(fname);
        exit(1);
    }

    return info.st_ino;
}

Stack* createStack()
{
    Stack* stack ;
    Node* node ;

    stack = (Stack*)malloc(sizeof(Stack));
    node = (Node*)malloc(sizeof(Node));

    node->link = node;
    node -> ino_num = -1;
    stack->list = node ;
    stack->cur = node ;

    return stack ;
}

Node* push(ino_t ino, Stack* stack)
{
    Node* node ;

    node = (Node*)malloc(sizeof(Node));

    node->ino_num = ino ;
    node->link = stack->cur;
    stack->cur = node ;

    return node ;
}

Node* pop(Stack* stack)
{
    Node* dNode ;
    dNode = stack->cur ;

    if(dNode == stack->list)
        return NULL;

    stack->cur = dNode->link;
    stack->list->link = stack->cur ;

    return dNode;
}
