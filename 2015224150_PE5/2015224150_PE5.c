#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>

int get_response(char* question);
void set_crmode();
int tty_mode(int how);

int main(void)
{
    void f(int);
    int i ;
    signal(SIGINT , f);
    for(i=0 ; i<10 ; i++)
    {
        printf("hello\n");
        sleep(1);
    }

    return 0;
}

void f(int signum)
{
    char response;
    char c;//temporary char

    printf("Interrupted! OK to quit (y/n)");
    scanf("%c", &response);

    //flush buffer
    do {
            c = getchar();
    }while (c != '\n' && c != EOF);


    if(response == 'y' || response == 'Y')
        exit(1);
    else if(response == 'n' || response == 'N');
}

