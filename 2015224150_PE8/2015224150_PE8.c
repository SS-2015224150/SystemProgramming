#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define READ 0
#define WRITE 1
#define oops(m,x) { perror(m); exit(x); }

FILE *popen2(const char *command, const char *mode)
{
        FILE *fp;
        int pid, p[2];

        if(pipe(p) == -1)
                oops("pipe faile", 1);
        if((pid = fork()) == -1)
                oops("cannot fork", 2);
        if(strcmp(mode, "r") == 0)
        {
                if(pid > 0)
                {
                        close(p[1]);
                        fp = fdopen(p[0], "r");
                        return fp;
                }
                else if( pid == 0)
                {
                        close(p[0]);
                        if(dup2(p[1], 1) == -1)
                                oops("error", 3);
                        close(p[1]);
                        execl("/bin/sh","sh","-c",command, NULL);
                }
        }
        if(strcmp(mode, "w") == 0)
        {
                if(pid > 0)

                {
                        close(p[0]);
                        fp = fdopen(p[1], "w");
                        return fp;
                }
                else if(pid == 0)
                {
                        close(p[1]);
                        if(dup2(p[0], 0) == -1)
                                oops("error", 3);
                        close(p[0]);
                        execl("/bin/sh", "sh", "-c", command, NULL);
                }
        }

}

main()
{
        FILE *fp;
        char buf[BUFSIZ];
	fp = popen2("ls", "r");
        while(fgets(buf, BUFSIZ, fp) != NULL)
                fputs(buf, stdout);
}
