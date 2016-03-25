#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

#define PATHLEN 1024

void do_ls(char dirname[] , int flag);
int dostat(char *filename);
void show_file_info(char *filename, struct stat *info_p);
void mode_to_letters(int mode, char str[]);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);
void sort_string_array(char (*array)[PATHLEN] , int);

char* PROGRAM_NAME ;
int main(int ac, char *av[])
{
    int flag = 0 ;
    PROGRAM_NAME = av[0];
    if(ac > 1 && strcmp(av[1] , "-R") == 0)
    {
        flag = 1;
        ++av ;
    }

    if(ac == 1)
        do_ls("." , flag);
    else if(ac == 2 && flag)
        do_ls("." , flag);
    else if(ac == 2)
    {
        do_ls(*av , flag);
    }
    else
        while(--ac)
        {
            do_ls(*(++av) , flag);
        }

    return 0;
}

void do_ls(char dirname[] , int flag)
{
    DIR *dir_ptr;
    struct dirent *direntp;
    int count , dirCount;
    int i;
    char path[PATHLEN];
    char arr[255][PATHLEN] ;
    char directory[255];

    if((dir_ptr = opendir(dirname)) == NULL){
        fprintf(stderr , "ls2: %s 디렉터리를 열 수 없음: 허가 거부\n" , dirname);
        return;
    }
    else
    {
        puts("");
        puts(dirname);
        count = 0;
        while((direntp = readdir(dir_ptr)) != NULL)
            strcpy(arr[count++] , direntp->d_name);

        closedir(dir_ptr);

        sort_string_array(arr , count);

        dirCount = 0;
        for(i=0 ; i<count ; i++)
        {
            strcpy(path , dirname);
            strcat(path , "/");
            strcat(path , arr[i]);
            if(dostat(path) == 1 && strcmp("." , arr[i]) != 0 && strcmp(".." , arr[i]) != 0)
                directory[dirCount++] = i;
        }
    }


    for(i=0; i<dirCount && flag ;i++)
    {
        strcpy(path , dirname);
        strcat(path , "/");
        strcat(path , arr[(int)directory[i]]);

        do_ls(path , 1);
    }

}

int dostat(char *filename)
{
    struct stat info;

    if(stat(filename, &info) == -1)
        perror(filename);
    else
        show_file_info(filename, &info);

    return S_ISDIR(info.st_mode);
}

void show_file_info(char *filename, struct stat *info_p)
{
    char *uid_to_name(), *ctime(), *gid_to_name(), *filemode();
    void mode_to_letters();
    char modestr[11];
    int i;

    mode_to_letters(info_p -> st_mode, modestr);

    printf("%s", modestr);
    printf("%4d ", (int) info_p -> st_nlink);
    printf("%-8s ", uid_to_name(info_p -> st_uid));
    printf("%-8s ", gid_to_name(info_p -> st_gid));
    printf("%8ld ", (long)info_p -> st_size);
    printf("%.12s ", 4+ctime(&info_p -> st_mtime));
    for(i = strlen(filename) - 1; i>=0 ; i--)
    {
        if(filename[i] == '/')
            break;
    }
    printf("%s\n", &filename[i+1]);

}

void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if(S_ISDIR(mode))
        str[0] = 'd';
    if(S_ISCHR(mode))
        str[0] = 'c';
    if(S_ISBLK(mode))
        str[0] = 'b';

    if(mode & S_IRUSR)
        str[1] = 'r';
    if(mode & S_IWUSR)
        str[2] = 'w';
    if(mode & S_IXUSR)
        str[3] = 'x';

    if(mode & S_IRGRP)
        str[4] = 'r';
    if(mode & S_IWGRP)
        str[5] = 'w';
    if(mode & S_IXGRP)
        str[6] = 'x';

    if(mode & S_IROTH)
        str[7] = 'r';
    if(mode & S_IWOTH)
        str[8] = 'w';
    if(mode & S_IXOTH)
        str[9] = 'x';
}

char *uid_to_name(uid_t uid)
{
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];

    if((pw_ptr = getpwuid(uid)) ==NULL)
    {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr -> pw_name;
}

char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(), *grp_ptr;
    static char numstr[10];

    if((grp_ptr = getgrgid(gid)) == NULL)
    {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_ptr -> gr_name;
}

void sort_string_array(char (*array)[PATHLEN] , int count)
{
    int i , j ;
    char path[PATHLEN] ;
    for(i = count-1 ; i > 0 ; i--)
    {
        for(j=0 ; j<i ; j++)
        {
            if(strcmp(array[j] , array[j+1]) > 0)
            {
                strcpy(path , array[j]);
                strcpy(array[j] , array[j+1]);
                strcpy(array[j+1] , path);
            }
        }
    }
}

