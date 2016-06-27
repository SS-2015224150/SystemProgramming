#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define HOSTLEN 256
#define BACKLOG 1

void read_til_crnl(FILE* fp);
void process_rq(char* rq , int fd) ;
void header(FILE* fp , char* content_type);
void cannot_do(int fd);
void do_404(char* item , int fd);
int isadir(char *f);
int not_exist(char* f);
void do_ls(char* dir , int fd) ;
char* file_type(char* f);
int ends_in_cgi(char* f);
void do_exec(char *prog , int fd) ;
void do_cat(char* f , int fd) ;
int make_server_socket(int portnum);
int make_server_socket_q(int portnum , int backlog);
int main(int ac , char* av[])
{
    int sock , fd ;
    FILE* fpin ;
    char request[BUFSIZ];

    if(ac == 1)
    {
        fprintf(stderr , "Usage: %s portnum\n" , av[0]);
        exit(1);
    }

    sock = make_server_socket(atoi(av[1]));
    if(sock == -1) exit(2);

    while(1)
    {
        fd = accept(sock , NULL , NULL);
        fpin = fdopen(fd , "r");

        fgets(request , BUFSIZ , fpin);

        printf("got a call : request = %s" , request) ;
        read_til_crnl(fpin);

        process_rq(request , fd);

        fclose(fpin);
    }

}

void read_til_crnl(FILE* fp)
{
    char buf[BUFSIZ] ;
    while(fgets(buf , BUFSIZ , fp) != NULL && strcmp(buf , "\r\n") != 0);
}

void process_rq(char* rq , int fd)
{
    char cmd[BUFSIZ] , arg[BUFSIZ] ;
    char *content ,*extension ; 
    FILE* fp ; 

    if(fork() != 0)
        return ;

    strcpy(arg , "./");
    if(sscanf(rq , "%s %s" , cmd , arg+2) != 2)
        return ;

    if(strcmp(cmd , "GET") != 0 && strcmp(cmd , "HEAD") != 0)
    {
		cannot_do(fd);
    }
    else if(not_exist(arg))
        do_404(arg , fd);
    else if(isadir(arg))
	{
		if(strcmp(cmd , "GET") == 0)
			do_ls(arg , fd);
		else
		{
			strcat(arg , "index.html");
			printf("modified arg : %s\n"  ,arg);
			if(not_exist(arg))
                    do_404(arg , fd);
		}
	}
    else if(ends_in_cgi(arg) && (strcmp(cmd , "GET") == 0))
        do_exec(arg ,fd);
    else if(strcmp(cmd , "GET") == 0)
        do_cat(arg , fd);

	if(strcmp(cmd , "HEAD") == 0)
	{
		fp = fdopen(fd , "w");

		extension = file_type(arg);
		content = "text/plain" ;

		if(strcmp(extension , "html") == 0)
			content = "text/html";
		else if(strcmp(extension , "gif") == 0)
			content = "image/gif";
		else if(strcmp(extension , "jpg") == 0)
			content = "image/jpeg";
		else if(strcmp(extension , "jpeg") == 0)
			content = "image/jpeg";
		else if(ends_in_cgi(arg))
			content = NULL ; 

		header(fp , content);

	
	}

	exit(0) ; 
}

void header(FILE* fp , char* content_type)
{
    fprintf(fp, "HTTP/1.0 200 OK\r\n");
    if(content_type)
        fprintf(fp , "Content-type: %s\r\n" , content_type);

}

void cannot_do(int fd)
{
    FILE* fp = fdopen(fd , "w") ;
    fprintf(fp , "HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp , "Content-type: text/plain\r\n");
    fprintf(fp , "\r\n");
    fprintf(fp , "That command is not yet implemented\r\n");
    fclose(fp);
}

void do_404(char* item , int fd)
{
    FILE* fp = fdopen(fd , "w");
    fprintf(fp , "HTTP/1.0 404 Not Found\r\n");
    fprintf(fp , "Content-type: text/plain\r\n");
    fprintf(fp , "\r\n");

    fprintf(fp , "The item you requested: %s\r\nis not found\r\n" , item);

    fclose(fp);
}

int isadir(char *f)
{
    struct stat info ;
    return (stat(f , &info) != -1 && S_ISDIR(info.st_mode));
}

int not_exist(char* f)
{
    struct stat info ;
    return (stat(f,&info) == -1);
}

void do_ls(char* dir , int fd)
{
    FILE* fp ;
    fp = fdopen(fd , "w");

    header(fp , "text/plain");
    fprintf(fp , "\r\n");
    fflush(fp) ;

    dup2(fd , 1);
    dup2(fd, 2);

    close(fd);
    execlp("ls" , "ls" , "-l" , dir , NULL);
    perror(dir);
    exit(1);

}

char* file_type(char* f)
{
    char* cp ;
    if((cp=strrchr(f , '.')) != NULL)
        return cp+1 ;
    return "" ;

}

int ends_in_cgi(char* f)
{
    return (strcmp(file_type(f) , "cgi") == 0);
}

void do_exec(char *prog , int fd)
{
    FILE* fp;
    fp = fdopen(fd , "w");
    header(fp , NULL);
    fflush(fp);
    dup2(fd , 1);
    dup2(fd , 2);
    close(fd);
    execl(prog , prog , NULL);
    perror(prog);
}

void do_cat(char* f , int fd)
{
    char *extension = file_type(f);
    char *content = "text/plain" ;
    FILE *fpsock , *fpfile ;
    int c ;

    if(strcmp(extension , "html") == 0)
        content = "text/html";
    else if(strcmp(extension , "gif") == 0)
        content = "image/gif";
    else if(strcmp(extension , "jpg") == 0)
        content = "image/jpeg";
    else if(strcmp(extension , "jpeg") == 0)
        content = "image/jpeg";

    fpsock = fdopen(fd , "w");
    fpfile = fopen(f , "r");
    if(fpsock != NULL && fpfile != NULL)
    {
        header(fpsock , content);
        fprintf(fpsock , "\r\n");
        while( (c = getc(fpfile)) != EOF)
            putc(c , fpsock);
        fclose(fpfile);
        fclose(fpsock);
    }

    exit(0);

}

int make_server_socket(int portnum)
{
    return make_server_socket_q(portnum , BACKLOG);
}

int make_server_socket_q(int portnum , int backlog)
{
    struct sockaddr_in saddr ;
    struct hostent *hp;
    char hostname[HOSTLEN];
    int sock_id ;

    sock_id = socket(PF_INET , SOCK_STREAM , 0) ;
    if(sock_id == -1)
    {
        return -1;
    }

    bzero((void*)&saddr , sizeof(saddr));
    gethostname(hostname , HOSTLEN);

    hp = gethostbyname(hostname);

//    bcopy((void*)hp->h_addr , (void*)&saddr.sin_addr , hp->h_length);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(portnum);
    saddr.sin_family = AF_INET;
    if(bind(sock_id , (struct sockaddr*)&saddr , sizeof(saddr)) != 0)
        return -1 ;

    if(listen(sock_id , backlog) != 0)
        return -1;
    return sock_id;
}
