#include <stdio.h>
#include <stdlib.h>

char *get_word()
{
	char t;
	char *word = NULL;
	int i = 0;
	t = getchar();

	while ((t == ' ') && (t != '0'))
		t = getchar();
	if (t == '0' || t == '\0' || t == '\n')
		return word;
	while (t != ' ' && t != '\n' && t != '\0' && t != '0')
	{
		word = (char*)realloc(word, (i + 1) * sizeof(char));
		word[i] = t;
		i++;
		t = getchar();
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


char **get_list()
{
	int i = 0;
	char **list = NULL;
	char *word = NULL;
	word = get_word();
	while (word != NULL)
	{
		//print_word(word);

		list = (char**)realloc(list, (i + 1) * sizeof(char));
		list[i] = word;
		i++;
		word = get_word();
	}
	list[i] = NULL;
	return list;
}

int main()
{
	char **list;
	char *word;
//	word = get_word();
//	print_word(word);
	list = get_list();
	print_list(list);
}
