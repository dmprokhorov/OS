#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
int main()
{
     printf("You're in the parent process with id [%i]\n", getpid());
     char symbol, *in = (char*)malloc(sizeof(char)), *file_path = (char*)malloc(sizeof(char));
     int *size = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0), counter = 0;
     if (size == MAP_FAILED)
     {
	     printf("Mapping failed in creation of integer value\n");
	     exit(1);
     }
     *size = 1;
     printf("Input path to the file\n");
     while ((symbol = getchar()) != '\n')
     {
	     file_path[counter++] = symbol;
             if (counter == *size)
             {
                     *size *= 2;
                     file_path = (char*)realloc(file_path, (*size) * sizeof(char));
             }
     }
     file_path = (char*)realloc(file_path, (counter + 1) * sizeof(char));
     file_path[counter] = '\0';
     counter = 0, *size = 1;
     printf("Now input some strings. If you want to end input, press Ctrl+D\n");
     while ((symbol = getchar()) != EOF)
     {
	     in[counter++] = symbol;
	     if (counter == *size)
	     {
		     *size *= 2;
		     in = (char*)realloc(in, (*size) * sizeof(char));
	     }
     }
     *size = counter + 1;
     in = (char*)realloc(in, (*size) * sizeof(char));
     in[(*size) - 1] = '\0';
     char* ptr = (char *)mmap(NULL, (*size)  * sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
     if(ptr == MAP_FAILED)
     {
     	     printf("Mapping failed in creation of array of chars\n");
	     free(in);
	     free(file_path);
	     int err = munmap(size, sizeof(int));
	     if (err != 0)
	     {
		     printf("Unmapping failed\n");
             }
             exit(1);
     }
     strcpy(ptr, in);
     int fd = open(file_path, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
     if (fd == -1)
     {
             printf("Failed to open file\n");
	     free(in);
	     free(file_path);
	     int err1 = munmap(ptr, (*size) * sizeof(char));
	     int err2 = munmap(size, sizeof(int));
             if ((err1 != 0) || (err2 != 0))
             {
                     printf("Unmapping failed\n");
             }
             exit(1);
     }
     char* f = (char *)mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     if (f == MAP_FAILED)
     {
             printf("Failed to create string associated with file\n");
	     free(in);
	     free(file_path);
	     int err1 = munmap(ptr, (*size) * sizeof(char));
	     int err2 = munmap(size, sizeof(int));
	     if ((err1 != 0) || (err2 != 0))
	     {
		     printf("Unmapping failed\n");
	     }
	     exit(1);
     }
     pid_t child_pid = fork();  
     if (child_pid == -1)
     {
 	     printf("Failed to create child process\n"); 
	     free(in);
	     free(file_path);
	     int err1 = munmap(ptr, (*size) * sizeof(char));
	     int err2 = munmap(size, sizeof(int));
             if ((err1 != 0) || (err2 != 0))
             {
                     printf("Unmapping failed\n");
             }
	     exit(1);
     }	     
     else if (child_pid == 0)
     {
	     //child
	     printf("You are in child process with id [%i]\n", getpid());
	     std::string string = std::string(), file_string = std::string(), out_string = std::string();
	     for (int i = 0; i < *size; i++)
             {
		     if (i != (*size) - 1)
		     {
                             string += ptr[i];
		     }
                     if ((ptr[i] == '\n') || (i == (*size) - 1))
                     {
                                if ((i > 0) && ((ptr[i - 1] == '.') || (ptr[i - 1] == ';')))
                                {
                                        file_string += string;
                                }
                                else
                                {
                                        out_string += string;
                                }
                                string = std::string();
                     }
             }
	     if ((file_string.length()) && (file_string[file_string.length() - 1] != '\n'))
	     {
		     file_string += '\n';
             }
	     if (file_string.length() != 0)
	     {
		     if ((ftruncate(fd, std::max((int)file_string.length(), 1) * sizeof(char))) == -1)
		     {
			     printf("Failed to truncate file\n");
			     free(in);
			     free(file_path);
			     return 1;
		     }
		     if ((f = (char *)mremap(f, sizeof(char), (file_string.length() + 1) * sizeof(char), MREMAP_MAYMOVE)) == ((void *)-1))
		     {
			     printf("Failed to resize memory for string associated with file\n");
			     free(in);
			     free(file_path);
			     return 1;	     
		     }     
		     sprintf(f, "%s", file_string.c_str());
	     }
	     if ((out_string.length()) && (out_string[out_string.length() - 1] != '\n'))
	     {
		     out_string += '\n';
	     }
	     if ((ptr = (char *)mremap(ptr, (*size) * sizeof(char), out_string.length() + 1, MREMAP_MAYMOVE)) == ((void *)-1))
	     {
		     printf("Failed to truncate file for string\n");
                     free(in);
                     free(file_path);
                     return 1;
	     }
	     *size = out_string.length() + 1;
	     sprintf(ptr, "%s", out_string.c_str());
     }
     else
     {
	     //parent
	     int wstatus;
	     waitpid (child_pid, &wstatus, 0);
	     if (wstatus)
	     {
	     	     free(in);
                     free(file_path);
		     int err1 = munmap(ptr, (*size) * sizeof(char));
		     int err2 = munmap(f, counter * sizeof(char));
		     int err3 = munmap(size, sizeof(int));
		     if ((err1 != 0) || (err2 != 0) || (err3 != 0))
		     {
			     printf("Unmapping failed\n");
		     }
		     exit(1);
	     }
	     printf("You are back in parent process with id [%i]\n", getpid());
	     struct stat statbuf;
	     if (fstat (fd, &statbuf) < 0)
             {
		     printf ("Problems with opening file %s\n", file_path);
                     exit(1);
             }
	     counter = std::max((int)statbuf.st_size, 1);
	     printf("These strings end in character '.' or ';':\n");
	     if (statbuf.st_size > 1)
	     {
	             printf("%s", f);
	     }
	     printf("------------------------------------------------------------------------------\nThese strings don't end in character '.' or ';':\n");
	     printf("%s", ptr);
	     close(fd);
	     int err1 = munmap(ptr, (*size) * sizeof(char));
	     int err2 = munmap(f, counter * sizeof(char));
	     int err3 = munmap(size, sizeof(int));
	     if ((err1 != 0) || (err2 != 0) || (err3 != 0))
	     {
		     printf("Unmapping failed\n");
		     free(in);
		     free(file_path);
		     exit(1);
	     }
     }
     free(in);
     free(file_path);
     return 0;
}
