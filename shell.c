#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>

pid_t *pids = NULL;
int num_of_processes = 0;
//int status = -3;


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
	if (t == '\n')
		return word;
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

void free_list(char **list, int pipe_count)
{
	int i = 0;
	if (list != NULL)
	{
		for (int j = 0; j <= pipe_count; j++)
		{
			while (list[i] != NULL)
			{
				free(list[i]);
				i++;
			}
			free(list[i]);
			i++;
		}
	}
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
	num_of_processes = pipe_count + 1;
	int fd_inp, fd_out;
	if (pipe_count > 0)
	{
		int fd[pipe_count][2];
	/*	int (*fd)[2];
		fd = malloc((pipe_count + 1) * sizeof(int [2]));*/
		for (int i = 0; i <= pipe_count; i++)
		{
			pipe(fd[i]);
			pids = realloc(pids, (i + 1) * sizeof(pid_t));
			pids[i] = fork();
			if (pids[i] == 0)
			{
				if (i > 0)
				{
					dup2(fd[i - 1][0], 0);
					close(fd[i - 1][0]);
					close(fd[i - 1][1]);
				}
				if (i == 0 && (input_name != NULL))
				{
					fd_inp = open(input_name, O_RDONLY, S_IRUSR|S_IWUSR);
					dup2(fd_inp, 0);
					close(fd_inp);
				}
				if (i < pipe_count)
				{
					dup2(fd[i][1], 1);
					close(fd[i][1]);
					close(fd[i][0]);
				}
				if (i == pipe_count && (output_name != NULL))
				{
					fd_out = open(output_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
					dup2(fd_out, 1);
					close(fd_out);
				}
				for (int j = 0; j <= i; j++)
 				{
					close(fd[j][0]);
					close(fd[j][1]);
				}
/*				close(fd[i][1]);
				close(fd[i][0]);
				close(fd[i - 1][1]);
				close(fd[i - 1][0]);
*/
				execvp(list[word_count], list + word_count);
				perror(list[word_count]);
				exit(1);
			} /*else{
				for (int j = 0; j <= i; j++)
 				{
					close(fd[j][0]);
					close(fd[j][1]);
				}
			}*/
			count_word(list, &word_count);
		}

		for (int j = 0; j <= pipe_count; j++)
		{
			close(fd[j][0]);
			close(fd[j][1]);
		}
//		if (input_name != NULL)
//			close(fd_inp);
///		close(fd_out);
//		close(fd[pipe_count - 1][0]);
//		close(fd[pipe_count - 1][1]);
		for (int j = 0; j <= pipe_count; j++)
		{
			wait(NULL);
		}

		return 0;
	}
	int wstatus = -3;
	if (input_name != NULL)
	{
		int fd;
		fd = open(input_name, O_RDONLY, S_IRUSR|S_IWUSR);
		if (fd < 0)
		{
				perror(input_name);
			return 1;
		}

		pids = realloc(pids, 1 * sizeof(pid_t));
		pids[0] = fork();
		if (pids[0] == 0)
		{
			printf (" fd  %d\n", fd);
			dup2(fd, 0);
			close(fd);
			execvp(list[0], list);
			exit(1);
			return 1;
		}
		wait(&wstatus);
		return wstatus;
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
		pids = realloc(pids, 1 * sizeof(pid_t));
		pids[0] = fork();
		if (pids[0] == 0)
		{
			dup2(fd2, 1);
			close(fd2);
			execvp(list[0], list);
		//	exit(1);
			return 1;
		}
	//	close(fd2);
		wait(&wstatus);
		return wstatus;
	}
	if (output_name == NULL)
	{
		pids = realloc(pids, 1 * sizeof(pid_t));
		pids[0] = fork();
		if (pids[0] == 0)
        	{
			execvp(list[0], list);
			exit(1);
			return 0;
	        }
	}
	wait(&wstatus);
	return wstatus;
}

void run_bg(char **list, pid_t *bg_pids, int *num_bg)
{
//	char *word;
//	char end;
//	word = get_word(&end);
	(*num_bg)++;
	*bg_pids = fork();
	if (*bg_pids == 0)
	{
		execvp(list[0], list);
		perror(list[0]);
		return;
	}
	printf("[ %d ]	%d\n", *num_bg, *bg_pids);
	return;
}

int change_directory(char *old_path, char *new_dir)
{
	char *new_path = NULL;
	if (chdir(new_dir))
	{
		perror("cd");
		return 1;
	}
	new_path = getcwd(new_path, strlen(old_path) + strlen(new_dir) + 2);
	setenv("OLDPWD", old_path, 1);
	setenv("PWD", new_path, 1);
	free(new_path);
	return 0;
}

int cd_command(char **list) {
	char *home = getenv("HOME");
	char *old_path = getenv("PWD");
	if (list[1] == NULL || !strcmp(list[1], "~")) {
		return change_directory(old_path, home);
	} else
	{
	if (!strcmp(list[1], "-"))
	{
		char *prev_dir = getenv("OLDPWD");
		if (prev_dir == NULL)
         		perror("cd");
		else
			return change_directory(old_path, prev_dir);
	} else
	{
		return change_directory(old_path, list[1]);
	}
	}
	return 0;
}

void get_trash()
{
	char end = ' ';
	while (end != '\n')
		get_word(&end);
	return;
}


char **get_list1(char *last_symbol, int *flag_end, int *pipe_count, char **list, pid_t *bg_pids,  int *num_bg)
{
	char *out_name = NULL;
	char *inp_name = NULL;
//	char **list = NULL;
	char pipe_sym[] = "|";
	char out_sym[] = ">";
	char inp_sym[] = "<";
	char bg_sym[] = "&";
	char cd_word[] = "cd";
	char and_word[] = "&&";
	char or_word[] = "||";
	*flag_end = 1;
	char end;
	int i = 0;
	do
	{
		list = realloc(list, (i + 1) * sizeof(char *));
		list[i] = get_word(&end);
		if (list[0] == NULL)
			continue;
		if (!strcmp(list[i], or_word))
		{
			list[i] = NULL;
			*flag_end = 1;
			if ((exec_command(list, *pipe_count, out_name, inp_name) > 0) && end != '\n')
			{
				out_name = NULL;
				inp_name = NULL;
				*pipe_count = 0;
				i = 0;
				free_list(list, *pipe_count);
				continue;
			}
			if (end != '\n')
				get_trash();
			return list;
		}
		if (!strcmp(list[i], and_word))
		{
			list[i] = NULL;
			*flag_end = 1;
			if ((exec_command(list, *pipe_count, out_name, inp_name) == 0) && end != '\n')
			{
				out_name = NULL;
				inp_name = NULL;
				*pipe_count = 0;
				i = 0;
				free_list(list, *pipe_count);
				continue;
			}
			if (end != '\n')
				get_trash();
			return list;
		}
		if (!strcmp(list[i], cd_word))
		{
			list = realloc(list, (i + 2) * sizeof(char *));
			if (end != '\n')
			{
	    			list[i + 1] = get_word(&end);
				list = realloc(list, (i + 3) * sizeof(char *));
				list[i + 2] = NULL;
			}
			else
				list[i + 1] = NULL;
			if (!cd_command(list) && end != '\n')
			{
				if (!strcmp(get_word(&end), and_word))
				{
					i = 0;
					*flag_end = 2;
					break;
				}
			}
			if (end != '\n')
				get_trash();
			return list;
		}
		if (!strcmp(list[i], bg_sym))
		{
			list[i] = NULL;
			run_bg(list, bg_pids, num_bg);
//			bg_flag = 1;
//			goto skip;
			return list;
		}
		if (!strcmp(list[i], pipe_sym))
		{
			(*pipe_count)++;
			list[i] = NULL;
			i++;
			continue;
		}
		if(!strcmp(list[i], out_sym))
		{
			out_name = get_word(&end);
			free(list[i]);
			if (end == '\n')
				break;
			continue;
		}
		if(!strcmp(list[i], inp_sym))
		{
			inp_name = get_word(&end);
			free(list[i]);
			if (end == '\n')
				break;
			continue;
		}
        	i++;
        }while ((end != '\n'));
	if (*flag_end == 1)
	{
		list = realloc(list, (i + 1) * sizeof(char *));
		*flag_end = 0;
		list[i] = NULL;
		exec_command(list, *pipe_count, out_name, inp_name);
		if (out_name != NULL)
			free(out_name);
		if (inp_name != NULL)
			free(inp_name);
	}
	return list;
}

void cmd_line_design()
{
	char *user = getenv("USER");
	char *working_directory = getenv("PWD");
	printf("%s" ":" "%s" "$ ", user, working_directory);
}


void handler(int signo)
{
	int i;
	putchar('\n');
	cmd_line_design();
	for (i = 0; i < num_of_processes; i++)
	{
		if (pids[i])
		{
			kill(pids[i], SIGINT);
        	}
		waitpid(pids[i], NULL, 0);
	}
}


int main()
{
	char **list = NULL;
	char last_sym;
	int pipe_count = 0; //кол-во |
	int flag_end = 0;
	int num_bg = 0;
	pid_t bg_pids;
	signal(SIGINT, handler);
	cmd_line_design();
	list = get_list1(&last_sym, &flag_end, &pipe_count, list, &bg_pids, &num_bg);
	while (!check_exit(list[0]))
	{
		free_list(list, pipe_count);
		pipe_count = 0;
		cmd_line_design();
		flag_end = 0;
		list = get_list1(&last_sym, &flag_end, &pipe_count, list, &bg_pids, &num_bg);



	}
	free_list(list, pipe_count);
	free(list);
	waitpid(bg_pids, NULL, 0);
//	free_list(list, 0);
	free(pids);
	return 0;
}

