/******************************************************
 * ex1a.c - Example, how to write a CGI program in C  *
 * for displaying information. On the page that this  *
 * program makes.                                     *
 ******************************************************
 * dev: Philip "5n4k3" Simonson                       *
 ******************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

/* #define DEBUG */
#define DATAFILE "data.dat"

void header(const char *mime_type);
void startup(const char *title);
void print_p(const char *paragraph);
void footer(const char *copyright);
char *getvar(char *s, const char *token, char ch);
int countfile(const char *filename);
int readfile(FILE *file, int i);
void test2(void);

int main(int argc, char *argv[])
{
	header("text/html");
	startup("Simple Webpage #2");
	test2();
	footer("Copyright (C) 2017");
	return 0;
}

void test2(void)
{
	FILE *file;
	int i, j, lines;
	char cwd[256];

	if(getcwd(cwd, 256) == NULL) {
		print_p("Cannot get the current working directory.");
		return;
	}
	strncat(cwd, "\\", 256);
	strncat(cwd, DATAFILE, 256);
	if((file = fopen(cwd, "rt")) == NULL) {
		print_p("Cannot read data file sorry :(");
		return;
	}
	printf("<center><table><caption>User List</caption><tr><th>Usernames</th><td>Identification</td></tr>");
	i = 0;
	lines = countfile(DATAFILE);
	while(j < lines) {
		readfile(file, i);
		i = !i;
	}

error:
	printf("</table></center>");
	fclose(file);
}

int countfile(const char *filename)
{
	FILE *file;
	int lines, c;

	if((file = fopen(filename, "rt")) == NULL) {
		print_p("Cannot open file.\n");
		return -1;
	}
	lines = 0;
	while((c = fgetc(file)) != EOF)
		if(c == '\n')
			++lines;
	fclose(file);
	return lines;
}

int readfile(FILE *file, int i)
{
	char data[256];

	memset(data, 0, sizeof(data));
	if(!i) {
		fgets(data, 256, file);
		printf("<tr><th>%s</th>");
	} else {
		fgets(data, 256, file);
		printf("<td>%s</td></tr>");
	}

	return 0;
}

void header(const char *mime_type)
{
	printf("Content-type:%s\n\n", mime_type);
}

void startup(const char *title)
{
	printf("<!DOCTYPE HTML><html><head><title>%s</title><style>header, footer { padding: 1em; " \
	"color: #7A7A7A; background-color: #2A2A2A; font-size: 15px; clear: center; text-align: center; }</style>" \
	"</head><body bgcolor=\"#585858\">", title);
	printf("<p><header>%s</header></p>", title);
}

void print_p(const char *paragraph)
{
	printf("<P>%s</P>", paragraph);
}

void footer(const char *copyright)
{
	printf("<p><footer>%s</footer></p>", copyright);
	printf("</body></html>");
}

char *strch(char *s, char ch)
{
	while(s && *s != 0)
		if(*s++ == ch)
			return s;
	return NULL;
}

char *__getvar;

char *getvar(char *s, const char *token, char ch)
{
	char *sbeg, *send;

	sbeg = s ? s : __getvar;
	if(!sbeg)
		return NULL;
	sbeg = strtok(s, token);
	if(!sbeg) {
		__getvar = NULL;
		return NULL;
	}
	send = strch(sbeg, ch);
	if(!send)
		return NULL;
	__getvar = send;
	return send;
}