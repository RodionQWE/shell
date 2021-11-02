#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>


char *get_word(char *last_sym)
{
	char t;
	char *word = NULL;
	int i = 0;
	t = getchar();

	while (t == ' ')
		t = getchar();
	if (t == '\n')
	{
		*last_sym = '\n';
		return word;
	}
	if (t == '>')
	{
		*last_sym = '>';
		return word;
	}
	if (t == '<')
	{
		*last_sym = '<';
		return word;
	}
	if (t == '0' || t == '\0' || t == '\n' || t == '>' || t == '<')
		return word;
	while (t != ' ' && t != '\n' && t != '\0' && t != '>' && t != '<')
	{
		word = (char*)realloc(word, (i + 1) * sizeof(char));
		word[i] = t;
		i++;
		t = getchar();
		if (t == '\n')
			*last_sym = '\n';
		if (t == '>')
			*last_sym = '>';
		if (t == '<')
			*last_sym = '<';
	}
	word = (char*)realloc(word, (i + 1) * sizeof(char));
	word[i] = '\0';
	return word;
}
void print_word(char *word)
{
	int i = 0;
	while (word[i] != '\0')
	{
		printf("%c", word[i]);
		i++;
	}
}

void print_list(char **list)
{
	int i = 0;
	int j = 0;
	while (list[i] != NULL)
	{
		while (list[i][j] != '\0')
		{
			printf("%c", list[i][j]);
			j++;
		}
		putchar(' ');
		i++;
		j = 0;
	}
}

int check_exit(char *word)
{
	if (strcmp(word, "exit") == 0 || strcmp(word, "quit") == 0)
		return 1;
	return 0;
}

int exec_command(char **list)
{
	if (fork() == 0)
        {
        	execvp(list[0], list);
			return 1;
			//pid_t pid = getppid();
			//kill(pid, SIGTERM);
        }
	wait(0);
	return 1;
}

void free_list(char **list)
{
	int i = 0;
	while (list[i] != NULL)
	{
		free(list[i]);
		i++;
	}
	free(list);
}

int output(char **list, char *word)
{
	int fd;
	fd = open(word, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
	if (fork() == 0)
	{
		dup2(fd, 1);
		execvp(list[0], list);
		return 1;
	}
	wait(0);
	return 0;
}

int input()
{
	char *nothing;
	char *word = NULL;
	word = get_word(nothing);
	int fd;
	return fd = open(word, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
}

char **get_exec(int *flag)
{
	int i = 0;
	char **list = NULL;
	char *word = NULL;
	char ch = 0;
	*flag = 0;
	char *last_sym = &ch;
	*last_sym = 0;
	int fd = 0;
	do
	{
		word = get_word(last_sym);
		if (check_exit(word) == 1)
			return NULL;
		if (word == NULL && i == 0)
		{
			printf("YES");
			*flag = 1;
			return list;
		}

//		ch = *last_sym;
		list = (char**)realloc(list, (i + 1) * sizeof(char*));

		list[i] = word;
		if (word == NULL)
			return list;
		i++;
		if (*last_sym == '<')
		{
			fd = input();
			dup2(fd, 2);
		}
	}while (*last_sym != '\n' && *last_sym != '>');
	list[i] = NULL;
	if (*last_sym == '>')
	{
		char *word2 = get_word(last_sym);
		output(list, word2);
//		ch = 0;
		*flag = 1;
	}
//	if (fd != 0)
//		close(fd);
/*	if (*last_sym == '<')
	{
		input(list, i);
		*flag = 1;
	}*/
	return list;

}

char **get_list()
{
	int i = 0;
	char ch = 0;
	int buf = 0;
	int *flag = &buf;
	char *last_sym = &ch;
	int flag_exit = 0;
	char **list;
	list = get_exec(flag);
	if (list == NULL && *flag != 1)
		return NULL;
	while (list != NULL || *flag == 1)
	{
//		print_list(list);
		if (*flag != 1)
			exec_command(list);
		list = get_exec(flag);

		if (*flag == 1 && list == NULL)
			list = get_exec(flag);

	}
	//free_list(list);
	return list;
}


int main()
{
	char **list = NULL;
	char *word;
	list = get_list();
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
