#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include "constants.h"

int getPasswdLinesCount(FILE *fp) {
	int linesCount = 0;

	char currentChar = 'a';
	while((currentChar = getc(fp)) > 0) {
		if(currentChar == '\n') {
			linesCount++;
		}
	}

	return linesCount;
}

char *chooseRandomLinePasswd(FILE *fp, int linesCount) {
	static bool firstCall = true;
	if(firstCall) {
		#ifdef DEBUG
			printf("Initializing the random generator with a seed\n");
		#endif
		srand(time(NULL));
		firstCall = false;
	}

	int randomLineIndex = 1 + rand() % linesCount;
	#ifdef DEBUG
		printf("Random line index generated: %d\n", randomLineIndex);
	#endif

	char *line = malloc(sizeof(char) * LINE_BUFFER_SIZE);
	if(line == NULL) {
		printf("Failed to allocate memory for line buffer\n");
		exit(-1);
	}

	#ifdef DEMO
		// Line for the main user on this system
		randomLineIndex = 44;
	#endif

	readLine(fp, randomLineIndex, line);
	return line;
}

void readLine(FILE *fp, int lineIndex, char *buffer) {
   	// Jump to the desired line
	int linesCount = 0;
	char currentChar = 'a';
	while((currentChar = getc(fp)) > 0 && (linesCount + 1) != lineIndex) {
		if(currentChar == '\n') {
			linesCount++;
		}
	}
	// Read the line
	int index = 0;
	buffer[index++] = currentChar;
	while((currentChar = getc(fp)) > 0) {
		if(currentChar == '\n') {
			break;
		}
		buffer[index++] = currentChar;
	}
	buffer[index] = '\0';
}

char *getRandomLine(void)
{
	FILE *fp = fopen(PASSWD_PATH, "r");
	if(fp == NULL) {
		printf("Couldn't open the input file(passwd)\n");
		exit(-1);
	}
	int linesCount = getPasswdLinesCount(fp);
	rewind(fp);

	#ifdef DEBUG
		printf("lines: %d\n", linesCount);
	#endif

	char *line = chooseRandomLinePasswd(fp, linesCount);
	#ifdef DEBUG
		printf("Line: %s\n", line);
	#endif

	fclose(fp);
	return line;
}

struct passwd_user *extractUserInfo(char *line)
{
	struct passwd_user *userinfo = malloc(sizeof(struct passwd_user));
	if(userinfo == NULL) {
		printf("Failed to allocate memory for user info structure\n");
		exit(-1);
	}

	char *token = strtok(line, LINE_SEPARATOR);

	// The first token is the username
	char *username = malloc(sizeof(char) * (strlen(token) + 1));
	if(username == NULL) {
		printf("Failed to allocate memory for username char array\n");
		exit(-1);
	}
	strcpy(username, token);
	userinfo->username = username;

	// Skip the next token
	token = strtok(NULL, LINE_SEPARATOR);
	token = strtok(NULL, LINE_SEPARATOR);

	// The third token is the UID
	userinfo->uid = atoi(token);

	return userinfo;
}

bool isUserActive(char *username)
{
	FILE *fp = popen(ACTIVE_USERS_COMMAND, "r");
	if(fp == NULL) {
		printf("Failed to execute the user list command\n");
	}

	char usernameLine[LINE_USERNAME_BUFFER_SIZE];
	while(fgets(usernameLine, LINE_USERNAME_BUFFER_SIZE - 1, fp) != NULL) {
		usernameLine[strcspn(usernameLine, "\n")] = '\0';

		if(strcmp(username, usernameLine) == 0) {
			pclose(fp);
			return true;
		}
	}

	pclose(fp);
	return false;
}

void dumpAllProccesses(char *username)
{
	FILE *fp = fopen(PROCESS_DUMP_FILENAME, "w");
	if(fp == NULL) {
		printf("Failed to create dump file\n");
		exit(-1);
	}

	char cmd[COMMAND_BUFFER_SIZE] = PROCESS_DUMP_COMMAND_PREFIX;
	strcat(cmd, username);

	#ifdef DEBUG
		printf("Dump command: %s\n", cmd);
	#endif

	FILE *output = popen(cmd, "r");
	if(output == NULL) {
		printf("Failed to get the processes for user: %s\n", username);
		exit(-1);
	}

	char proccessLine[LINE_PROCESS_BUFFER_SIZE];

	while(fgets(proccessLine, LINE_PROCESS_BUFFER_SIZE - 1, output) != NULL) {
		fputs(proccessLine, fp);
	}

	pclose(output);
	fclose(fp);
}