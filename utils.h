#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

struct passwd_user {
	int uid;
	char *username;
};

int getPasswdLinesCount(FILE *fp);
char *chooseRandomLinePasswd(FILE *fp, int lineCount);
void readLine(FILE *fp, int lineIndex, char *buffer);
char *getRandomLine(void);
struct passwd_user *extractUserInfo(char *line);
bool isUserActive(char *username);
void dumpAllProccesses(char *username);


#endif