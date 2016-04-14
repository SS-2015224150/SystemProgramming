#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define MAXPATH 512

void findFilename(char* fullPath , char * filename);
int get_directory_ino(char *fullpath);

int main(int argc,char *argv[])
{
    int fd1,fd2,count;
    char buf[MAXPATH] , buf2[MAXPATH];
    char argv1_filename[MAXPATH];
    char fullpath[MAXPATH];
    struct stat info;

    //error control
    if(argc!=3)
    {
        fprintf(stderr, "Argument error!\n");
        exit(1);
    }
    else
    {
        stat(argv[2], &info);

	findFilename(argv[1] , buf);
	findFilename(argv[2] , buf2);

        //same file name both source file and new file
        if(strcmp(buf , buf2) == 0 && get_directory_ino(argv[1]) == get_directory_ino(argv[2]))
        {
            fprintf(stderr, "The same filename!\n");
            exit(1);
        }
        else
        {
            //source file open
            fd1=open(argv[1], O_RDONLY);

            if(fd1==-1)
            {
                fprintf(stderr, "File does not exist");
                exit(1);
            }

            //argv[2] = diretory
            if(S_ISDIR(info.st_mode))
            {

                //get Fullpath
                strcpy(fullpath , argv[2]);
                findFilename(argv[1] , argv1_filename);
                if(fullpath[strlen(fullpath)-1] != '/')
                    strcat(fullpath , "/");

                strcat(fullpath , argv1_filename);

                fd2=open(fullpath, O_WRONLY | O_CREAT , 0777);

                //copy the content of source file to new file
                while((count = read(fd1, buf, 512)) > 0)
                {
                    write(fd2, buf, count);
                }
            }

            //argv[2] = file
            else
            {
                //new file open
                fd2=open(argv[2], O_WRONLY | O_CREAT, 0777);

                //copy the content of source file to new file
                while((count = read(fd1, buf, 512)) > 0)
                {
                    write(fd2, buf, count);
                }
            }
            //files close
            close(fd1);
            close(fd2);
        }
    }
    //remove source file
    int result = unlink(argv[1]);

    if(result == -1)
        fprintf(stderr, "Fail to remove source file");

     printf("the Command is successfully operated\n");

    return 0;
}
//get only filename
void findFilename(char *fullPath , char *filename)
{
    int idx ;

    idx = strlen(fullPath);

    for(idx -- ; idx > -1 ; idx--)
    {
        if(fullPath[idx] == '/')
            break;
    }

    if(idx == -1)
        strcpy(filename , fullPath);
    else
        strcpy(filename , &fullPath[idx+1]);
}

int get_directory_ino(char *fullpath)
{
    char dir_name[MAXPATH];
    struct stat st ;
    int findDirectory(char* , char*);
    
    if(lstat(fullpath , &st) == -1)
        return -1 ; 
        
    if(S_ISDIR(st.st_mode))
        return st.st_ino ; 
    
    if(findDirectory(fullpath , dir_name) == -1)
        return -1 ; 
            
    if(lstat(dir_name , &st) == -1)
        return -1 ; 
        
    if(S_ISDIR(st.st_mode))
        return st.st_ino ; 
}

int findDirectory(char *src , char *dst)
{
    int idx ;

    idx = strlen(src);

    for(idx -- ; idx > -1 ; idx--)
    {
        if(src[idx] == '/')
            break;
    }

    if(idx == -1){
        return -1;
    }
    if(idx == -1){
        return -1;
    }
         
        
    strcpy(dst , src);
    
    dst[idx] = 0 ;
    
    return 0;    
}












































