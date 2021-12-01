#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>

void print_word(char *word)
{
	int i = 0;
	while (word[i] != '\0')
	{
		printf("%c", word[i]);
		i++;
	}
}

char *get_word(char *last_sym) {
        char *word = NULL, t;
        int n = 0;
        t = getchar();
	while (t == ' ')
		t = getchar();
	while (t != ' ' && t != '\t' && t != '\n')
	{
		word = realloc(word, (n + 1) * sizeof(char));
		word[n] = t;
		n++;
		t = getchar();
	}
	word = realloc(word, (n + 1) * sizeof(char));
	word[n] = '\0';
	*last_sym = t;
	return word;
}

void count_word(char **list, int *word_count)
{
	int i = 0;
	while (list[i + *word_count] != NULL)
	{
		i++;
	}
	(*word_count) = (*word_count) + i + 1;

}
void print_list(char **list, int *word_count)
{
	int i = 0;
	int j = 0;
	while (list[i + *word_count] != NULL)
	{
		while (list[i + *word_count][j] != '\0')
		{
			printf("%c", list[i + *word_count][j]);
			j++;
		}
		putchar(' ');
		i++;
		j = 0;
	}
	(*word_count) = (*word_count) + i + 1;
}

int check_exit(char *word)
{
	if (strcmp(word, "exit") == 0 || strcmp(word, "quit") == 0)
		return 1;
	return 0;
}

int exec_command(char **list, int pipe_count, char *output_name, char *input_name)
{
	int word_count = 0;
	if (pipe_count > 0)
	{
		int (*fd)[2];
		fd = malloc((pipe_count + 1) * sizeof(int [2]));
		for (int i = 0; i <= pipe_count; i++)
			pipe(fd[i]);
		for (int i = 0; i <= pipe_count; i++)
		{
			if (fork() == 0)
			{
				if (i > 0)
					dup2(fd[i - 1][0], 0);
				if (i == 0 && (input_name != NULL))
				{
					int fd_inp = open(input_name, O_RDONLY, S_IRUSR|S_IWUSR);
					dup2(fd_inp, 0);
					close(fd_inp);
				}
				if (i < pipe_count)
					dup2(fd[i][1], 1);
				if (i == pipe_count)
				{
					int fd_out = open(output_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
					dup2(fd_out, 1);
					close(fd_out);
				}
				if (i > 0)
				{
					close(fd[i - 1][0]);
					close(fd[i - 1][1]);
				}
				close(fd[i][1]);
				close(fd[i][0]);
				execvp(list[word_count], list + word_count);
				exit(1);
			}
			count_word(list, &word_count);
		for (int j = 0; j < i; j++)
 		{
			close(fd[j][0]);
			close(fd[j][1]);
		}
		}
		for (int j = 0; j <= pipe_count; j++)
		{
			close(fd[j][0]);
			close(fd[j][1]);
		}

		wait(0);
		wait(0);
		return 0;
	}
	if (input_name != NULL)
	{
		int fd;
		fd = open(input_name, O_RDONLY, S_IRUSR|S_IWUSR);
		if (fd < 0)
		{
			perror(input_name);
			return 1;
		}

		if (fork() == 0)
		{
			printf (" fd  %d\n", fd);
			dup2(fd, 0);
			close(fd);
			execvp(list[0], list);
			exit(1);
			return 1;
		}
		wait(0);
		return 1;
	}
	if (output_name != NULL)
	{
		int fd2;
		fd2 = open(output_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
		if (fd2 < 0)
		{
			perror(output_name);
			return 1;
		}
		if (fork() == 0)
		{
			dup2(fd2, 1);
			close(fd2);
			execvp(list[0], list);
			exit(1);
			return 1;
		}
		wait(0);
		return 1;
	}
	if (output_name == NULL)
	{
		if (fork() == 0)
        	{
        		execvp(list[0], list);
			exit(1);
			wait(0);
	        }
	}
	wait(0);
	return 1;
}

char **get_list1(char *last_symbol)
{
	char *out_name = NULL;
	char *inp_name = NULL;
	char **list = NULL;
	char pipe_sym[] = "|";
	char out_sym[] = ">";
	char inp_sym[] = "<";
	char end;
	char end2;
	int pipe_count = 0;
	char *word;
	int i = 0; //,new_pipe_num = *old_pipe_num;
	do
	{
		start:
		list = realloc(list, (i + 1) * sizeof(char *));
		start_start:
		list[i] = get_word(&end);
		if (!strcmp(list[i], pipe_sym))
		{
			pipe_count++;
			list[i] = NULL;
			i++;
			goto start;
		}
		if(!strcmp(list[i], out_sym))
		{
			out_name = get_word(&end);
			if (end == '\n')
				goto skip;
			goto start_start;
		}
		if(!strcmp(list[i], inp_sym))
		{
			inp_name = get_word(&end);
		//	inp_name = NULL;
			if (end == '\n')
				goto skip;
			else
			{
		//		putchar('N');
				goto start_start;
			}
		}
        	i++;
        }while ((end != '\n'));
	list = realloc(list, (i + 1) * sizeof(char *));
	skip:
	list[i] = NULL;
	exec_command(list, pipe_count, out_name, inp_name);
	return list;
}


void free_list(char **list)
{
	int i = 0;
	if (list != NULL)
	{
		while (list[i] != NULL)
		{
			free(list[i]);
			i++;
		}
	//	free(list[i]);
	}
	free(list);
}

char **get_list2()
{
	char **list = NULL;
	char last_sym;
	list = get_list1(&last_sym);
	if (list == NULL)
		return list;
	while (!check_exit(list[0]))
	{

		list = get_list1(&last_sym);

	}
	return list;
}


int main()
{
	char **list = NULL;
	get_list2();
//	char *word;
/*	int pipe_count = 0;
	char last_sym;
	int word_count = 0;
	list = get_list1(&last_sym, &pipe_count);
	for (int i = 0; i < pipe_count + 1; i++)
	{
		print_list(list, &word_count);
	}*/
//	free_list(list);
//	exec_command(list);
//	list = get_list();
//	exec_command(list);
//	print_list(list);
//	free_list(list);
//	list = NULL;
//	list = get_exec(list);
//	print_list(list);
	return 0;
}

