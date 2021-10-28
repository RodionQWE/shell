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
		*last_sym = '\n';
	if (t == '0' || t == '\0' || t == '\n')
		return word;
	while (t != ' ' && t != '\n' && t != '\0')
	{
		word = (char*)realloc(word, (i + 1) * sizeof(char));
		word[i] = t;
		i++;
		t = getchar();
		if (t == '\n')
			*last_sym = '\n';
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

void exec_command(char **list)
{
	if (fork() == 0)
        {
                execvp(list[0], list);
        }
        wait(0);
}

void free_list(char **list)
{
	int i = 0;
	while (list[i] != NULL)
	{
		free(list[i]);
		i++;
	}
	free(list[i]);
	free(list);
}

char **get_exec()
{
	int i = 0;
	char ch = 0;
	char **list = NULL;
	char *word = NULL;
	char *last_sym = &ch;
	do
	{
		word = get_word(last_sym);
		if (check_exit(word) == 1)
			return NULL;
		ch = *last_sym;
		list = (char**)realloc(list, (i + 1) * sizeof(char));
		list[i] = word;
		i++;

	}while (ch != '\n');
//	ch = 0;
//	free(last_sym);
	list[i] = NULL;
	return list;

}

char **get_list()
{
	int i = 0;
	char ch;
	char *last_sym = &ch;
	int flag_exit = 0;
	char **list;
	list = get_exec();
	if (list == NULL)
		return NULL;
	while (list != NULL)
	{
//		print_list(list);
		exec_command(list);
		list = get_exec();

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
	free_list(list);
//	list = NULL;
//	list = get_exec(list);
//	print_list(list);
	return 0;
}
