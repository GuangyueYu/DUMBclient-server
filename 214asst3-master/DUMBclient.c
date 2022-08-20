#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void substr(char *str, char *sub, int start, int end);
char *getInput(int size);

char *sendMessage(int server, char *msg) {
	write(server, msg, strlen(msg) + 1);
	
	char *buffer = (char *) malloc(256);
	read(server, buffer, 256);
	return buffer;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("ERROR: Invalid number of arguments.\n");
		return 1;
	}

	int servSock = socket(AF_INET, SOCK_STREAM, 0);
	if(servSock == -1) {
		perror("An error occured while creating socket.\n");
		return 1;
	}
	
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	
	if(inet_pton(AF_INET, argv[1], &address.sin_addr) < 1) {
		printf("ERROR: Invalid IP address.\n");
		return 1;
	}
	
	int i;
	for(i = 0; i < 3; i++) {
		printf("Connecting (%d/3)...\n", i + 1);
		if(connect(servSock, (struct sockaddr *) &address, sizeof(address)) == -1) {
			perror("An error occured while connecting to server.\n");
		} else {
			break;
		}
	}
	
	if(i == 3) {
		printf("Failed to connect, exiting\n");
		return 1;
	}
	
	if(strcmp(sendMessage(servSock, "HELLO"), "HELLO DUMBv0 ready!") == 0) {
		printf("Connected successfully.\n");
	} else {
		printf("An error occured on server side. Closing the connection.\n");
		return 1;
	}
	
	printf("Enter your commands below.\n");
	char *command = (char *) malloc(1<<16);
	printf("> ");
	command = getInput(1<<16);
	
	char *boxName = "";
	
	while (1) {
		if (strcmp(command, "help") == 0){
			printf("These are the possible commands:\n");
			printf("quit\ncreate\ndelete\nopen\nclose\nnext\nput\n");								
		} else if(strcmp(command, "quit") == 0) {
			if(strlen(sendMessage(servSock, "GDBYE")) == 0) {
				printf("Disconnected successfully. Goodbye!\n");
				break;
			} else {
				printf("An error occured while disconnecting.\n");
			}
		} else if(strcmp(command, "create") == 0) {
			printf("Okay, give the name of the message box to be created.\n");
			
			char *name = (char *) malloc(sizeof(char) * 25);
			printf("create:> ");
			name = getInput(25);
			
			char *createMessage = (char *) malloc(sizeof(char) * 31);
			strcpy(createMessage, "CREAT ");
			strcat(createMessage, name);

			if(strlen(name) >= 5 && strlen(name) <= 25) {
				if(tolower(name[0]) >= 'a' && tolower(name[0]) <= 'z') {
					char *response = sendMessage(servSock, createMessage);
					if(strcmp(response, "OK!") == 0) {
						printf("Success! Message box is created.\n");
					} else if(strcmp(response, "ER:EXIST") == 0) {
						printf("ERROR: message box already exists.\n");
					} else if(strcmp(response, "ER:WHAT?") == 0) {
						printf("ERROR: command is malformed.\n");
					}
				} else {
					printf("ERROR: Message box name must begin with a letter.\n");
				}
			} else {
				printf("ERROR: Message box name must be between 5-25 characters.\n");
			}
		} else if(strcmp(command, "delete") == 0) {
			printf("Okay, give the name of the message box to be deleted.\n");
			
			char *name = (char *) malloc(sizeof(char) * 25);
			printf("delete:> ");
			name = getInput(25);
			
			char *deleteMessage = (char *) malloc(sizeof(char) * 31);
			strcpy(deleteMessage, "DELBX ");
			strcat(deleteMessage, name);
			
			char *response = sendMessage(servSock, deleteMessage);
			if(strcmp(response, "OK!") == 0) {
				printf("Success! Message box is deleted.\n");
			} else if(strcmp(response, "ER:NEXST") == 0) {
				printf("ERROR: message box does not exist.\n");
			} else if(strcmp(response, "ER:OPEND") == 0) {
				printf("ERROR: message box is already opened.\n");
			} else if(strcmp(response, "ER:NOTMT") == 0) {
				printf("ERROR: message box is not empty.\n");
			} else if(strcmp(response, "ER:WHAT?") == 0) {
				printf("ERROR: command is malformed.\n");
			}
		} else if(strcmp(command, "open") == 0) {
			printf("Okay, give the name of the message box to be opened.\n");
			
			char *name = (char *) malloc(sizeof(char) * 25);
			printf("open:> ");
			name = getInput(25);
			
			char *openMessage = (char *) malloc(sizeof(char) * 31);
			strcpy(openMessage, "OPNBX ");
			strcat(openMessage, name);
			
			char *response = sendMessage(servSock, openMessage);
			if(strcmp(response, "OK!") == 0) {
				boxName = name;
				printf("Success! Message box is opened.\n");
			} else if(strcmp(response, "ER:NEXST") == 0) {
				printf("ERROR: message box does not exist.\n");
			} else if(strcmp(response, "ER:OPEND") == 0) {
				printf("ERROR: message box is already opened.\n");
			} else if(strcmp(response, "ER:ONLY1") == 0) {
				printf("ERROR: only one message box can be opened at a time.\n");
			} else if(strcmp(response, "ER:WHAT?") == 0) {
				printf("ERROR:command is malformed.\n");
			}
		} else if(strcmp(command, "close") == 0) {
			printf("Okay, give the name of the message box to be closed.\n");
			
			char *name = (char *) malloc(sizeof(char) * 25);
			printf("close:> ");
			name = getInput(25);
			
			char *closeMessage = (char *) malloc(sizeof(char) * 31);
			strcpy(closeMessage, "CLSBX ");
			strcat(closeMessage, name);
			
			char *response = sendMessage(servSock, closeMessage);
			if(strcmp(response, "OK!") == 0) {
				boxName = "";
				printf("Success! Message box is closed.\n");
			} else if(strcmp(response, "ER:NOOPN") == 0) {
				printf("ERROR: message box not currently open or does not exist.\n", boxName);
			} else if(strcmp(response, "ER:WHAT?") == 0) {
				printf("ERROR: command is malformed.\n");
			}
		} else if(strcmp(command, "next") == 0) {
			char *response = sendMessage(servSock, "NXTMG");
			char *sub = (char *) malloc(6);
			substr(response, sub, 0, 3);
			
			if(strcmp(sub, "OK!") == 0) {
				int len;
				char buffer[1<<16];
				
				sscanf(response, "OK!%d!%[^\n]s", &len, buffer);
				printf("%s\n", buffer);
			} else if(strcmp(response, "ER:EMPTY") == 0) {
				printf("ERROR: message box is empty.\n");
			} else if(strcmp(response, "ER:NOOPN") == 0) {
				printf("ERROR: no message box is currently opened.\n");			
			} else if(strcmp(response, "ER:WHAT?") == 0) {
				printf("ERROR: command is malformed.\n");
			}
		} else if(strcmp(command, "put") == 0) {
			printf("Okay, enter the message to be sent.\n");
			
			char *msg = (char *) malloc(1<<16);
			printf("put:> ");
			msg = getInput(1<<16);
			
			char *putMessage = (char *) malloc(1<<16);
			sprintf(putMessage, "PUTMG!%d!%s", strlen(msg), msg);
			
			char *response = sendMessage(servSock, putMessage);
			char *sub = (char *) malloc(4);
			substr(response, sub, 0, 3);
			
			if(strcmp(sub, "OK!") == 0) {
				printf("Success! Message is sent.\n");
			} else if(strcmp(response, "ER:NOOPN") == 0) {
				printf("ERROR: no message box is currently opened.\n");
			} else if(strcmp(response, "ER:WHAT?") == 0) {
				printf("ERROR: command is malformed.\n");
			}
		} else {
			printf("That is not a command, for a command list enter 'help'.\n");
		}
		
		printf("> ");
		command = getInput(1<<16);
	}
	
	return 0;
}

void substr(char *str, char *sub, int start, int end) {
    memcpy(sub, &str[start], end - start);
    sub[end - start] = '\0';
}

char *getInput(int size) {
	char *buffer = (char *) malloc(size);
	scanf("%[^\n]", buffer);
	buffer = strtok(buffer, "\n");
	
	char c;
	while((c = getchar()) != '\n' && c != EOF);
	
	return buffer;
}
