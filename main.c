#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "utils.h"
#include "constants.h"

int main(void) {
	char *line = getRandomLine();

	int fid[2];
	if(pipe(fid) < 0) {
		printf("Failed to init pipe\n");
	}

	pid_t pid = fork();
	if(pid == -1) {
		printf("Error while creating child process\n");
	} else if(pid == 0) { 
		char childLine[LINE_BUFFER_SIZE];
		int result = read(fid[0], childLine, LINE_BUFFER_SIZE);
		if(result == 0) {
			printf("Something went wrong when reading from pipe\n");
		}
		#ifdef DEBUG
			printf("Child read: %s\n", childLine);
		#endif

		struct passwd_user *user = extractUserInfo(childLine);
		printf("Username: %s\n", user->username);
		printf("UID: %d\n", user->uid);

		if(isUserActive(user->username)) {
			printf("User is active, dumping processes to file: %s\n", PROCESS_DUMP_FILENAME);
			dumpAllProccesses(user->username);
		} else {
			printf("User is not active, no dumping will occur\n");
		}

		free(user->username);
		free(user);
	} else {
		int lineLength = strlen(line);
		int writtenBytes = write(fid[1], line, lineLength + 1);
		if(writtenBytes - 1 != lineLength) {
			printf("Something went wrong while writing line to pipe\n");
			printf("Difference: %d %d\n", lineLength, writtenBytes);
			exit(-1);
		}
		wait(NULL);	
	}

	free(line);
	return 0;
}