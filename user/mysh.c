#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define MAX_WORD 25 // max length of an individual word/command
#define MAX_INPUT 100 // max length of a user-entered string/command

int exit_flag; //terminates mysh

struct command {
    char *argv[MAX_INPUT];
    int argc; 
};

struct redirect_command {
    char *redirect_argv[MAX_INPUT];
    int redirect_argc;
};

struct pipe_command {
    char *left[MAX_INPUT/2];
    char *right[MAX_INPUT/2];
    int argl;
    int argr;
};

struct command *cmd, cmd1;
struct redirect_command *red_cmd, red_cmd1;
struct pipe_command *pipe_cmd, pipe_cmd1;

void cleanup() {
    for(int i = 0; i < cmd->argc; i++) {
        free(cmd->argv[i]);
    }
    cmd->argc = 0;

    for(int i = 0; i < red_cmd->redirect_argc; i++) {
        free(red_cmd->redirect_argv[i]);
    }
    red_cmd->redirect_argc = 0;

    for (int i = 0; i < pipe_cmd->argl; i++) {
        free(pipe_cmd->left[i]);
    }
    pipe_cmd->argl = 0;

    for (int i = 0; i < pipe_cmd->argr; i++) {
        free(pipe_cmd->right[i]);
    }
    pipe_cmd->argr = 0;
}

// reading user-entered strings/commands
void read_input(struct command *cmd) {
    int count = 0; // count for the characters in a word
    char temp[MAX_WORD];
    char current_char;
    while(read(STDIN, &current_char, sizeof(char)) == 1) {
        if (current_char != ' ' && current_char != '\n') {
            temp[count] = current_char;
            count++;
        } else {
            temp[count] = '\0';
            cmd->argv[cmd->argc] = (char*) malloc (sizeof(temp));
            strcpy(cmd->argv[cmd->argc], temp);
            cmd->argc++;
            count = 0;
            if (current_char == '\n') {
                break;
            }
        }
    }
    cmd->argv[cmd->argc] = (char*) malloc (sizeof(0x0));
    cmd->argv[cmd->argc] = 0x0;
    printf("********** read_input executed ***************\n");
}

void parse_input(struct command *cmd, struct redirect_command *red_cmd, struct pipe_command *pipe_cmd) {
    for (int i = 0; i < cmd->argc-1; i++) {
        if (strcmp(cmd->argv[i], "<") != 0 && strcmp(cmd->argv[i], ">") != 0 && strcmp(cmd->argv[i], "|") != 0) {
            red_cmd->redirect_argv[red_cmd->redirect_argc] = (char*) malloc (sizeof(cmd->argv[i]));
            strcpy(red_cmd->redirect_argv[red_cmd->redirect_argc], cmd->argv[i]);
            red_cmd->redirect_argc++;
        } 
    }
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "|") == 0) {
            for (int j = 0; j < i; j++) {
                pipe_cmd->left[pipe_cmd->argl] = (char*) malloc (sizeof(cmd->argv[j]));
                strcpy(pipe_cmd->left[j], cmd->argv[j]);
                pipe_cmd->argl++;
            }
            int r = 0;
            for (int k = i+1; k < cmd->argc; k++) {
                pipe_cmd->right[pipe_cmd->argr] = (char*) malloc (sizeof(cmd->argv[k]));
                strcpy(pipe_cmd->right[r], cmd->argv[k]);
                pipe_cmd->argr++;
                r++;
            }
        }
        red_cmd->redirect_argv[red_cmd->redirect_argc] = (char*) malloc (sizeof(cmd->argv[0x0]));
        red_cmd->redirect_argv[red_cmd->redirect_argc] = 0x0;
        pipe_cmd->left[pipe_cmd->argl] = (char*) malloc (sizeof(0x0));
        pipe_cmd->left[pipe_cmd->argl] = 0x0;
        pipe_cmd->right[pipe_cmd->argr] = (char*) malloc (sizeof(0x0));
        pipe_cmd->right[pipe_cmd->argr] = 0x0;
    }

    printf("\n*********** TESTING REDIRECT ***********\n");

        for (int  i = 0; i < red_cmd->redirect_argc; i++) {
            printf("redirect_argv at %d is %s\n", i, red_cmd->redirect_argv[i]);
        }
        printf("redirect_argc is %d\n", red_cmd->redirect_argc);

    printf("\n*********** TESTING PIPES ***********\n");

        for (int  i = 0; i < pipe_cmd->argl; i++) {
            printf("left at %d is %s\n", i, pipe_cmd->left[i]);
        }
        printf("argl is %d\n", pipe_cmd->argl);
        for (int  i = 0; i < pipe_cmd->argr; i++) {
            printf("right at %d is %s\n", i, pipe_cmd->right[i]);
        }
        printf("argr is %d\n", pipe_cmd->argr);
}

// handles simple I/O redirection
void redirect(struct command *cmd, struct redirect_command *t_cmd) {
    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            if (fork() == 0) {
                close(STDIN);
                int fd = open(cmd->argv[cmd->argc-1], O_RDONLY);
                exec(cmd->argv[0], red_cmd->redirect_argv);
                close(fd);
            } else {
                wait(0);
            }
        } else if (strcmp(cmd->argv[i], ">") == 0) {
            if (fork() == 0) {
                close(STDOUT);
                int fd = open(cmd->argv[cmd->argc-1], O_CREATE | O_WRONLY);
                exec(cmd->argv[0], red_cmd->redirect_argv);
                close(fd);
            }
            else {
                wait(0);
            }
        }
    }
}

void pipes(struct pipe_command *pipe_cmd) {
    int p[2];
    pipe(p);
    if (fork() == 0) { //lhs write
        close(p[STDIN]);
        close(STDOUT);
        dup(p[STDOUT]);
        close(p[STDOUT]);
        exec(pipe_cmd->left[0], pipe_cmd->left);
    }
    if (fork() == 0) { //rhs read
        close(STDIN);
        close(p[STDOUT]);
        dup(p[STDIN]);
        close(p[STDIN]);
        exec(pipe_cmd->right[0], pipe_cmd->right);
    }
    close(p[STDIN]);
    close(p[STDOUT]);
    wait(0);
    wait(0);
}

/*void multi_pipes() {
    ls | cat | cat | cat
    int indices[20];
    int p[2];
    int index = 0;
    int in = STDIN; //this is our input for the next child
    int out = STDOUT; //output for each child
    int n = cmd->argc;
    for (int i = 0; i < n; i++) {
        if (strcmp(cmd->argv[i], "|") == 0) {
            indices[index] = i;
            index++;
        }
    }
    int index2 = 0;
    for (int i = 0; i < cmd->argc; i++) 
        if (i == indices[index2]) 
    
}*/


//executing the user input for simple commands
void exec_input(struct command *cmd) {
    if(strcmp(cmd->argv[0], "cd") != 0 && strcmp(cmd->argv[0], "exit") != 0) {
        if (fork() == 0) {
            printf("*********** PARSE %s executed ***************\n", cmd->argv[0]);
            if (exec(cmd->argv[0], cmd->argv) == -1) {
                printf("Error: %s could not be executed.\n", cmd->argv[0]);
            }
        } else {
            wait(0);
        }
    } else if (strcmp(cmd->argv[0], "cd") == 0) {
        printf("Moved to %s \n", cmd->argv[1]);
        chdir(cmd->argv[1]);
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        printf("Exiting mysh...\n");
        exit_flag = -1;
        printf("*********** PARSE %s executed ***************\n", cmd->argv[0]);
    }
}

int main() {

    cmd = &cmd1;
    red_cmd = &red_cmd1;
    pipe_cmd = &pipe_cmd1;
    cmd->argc = 0;
    red_cmd->redirect_argc = 0;
    pipe_cmd->argl = 0;
    pipe_cmd->argr = 0;

    while (exit_flag != -1) {
        exit_flag = 0;
        int redir_counter = 0;

        printf("\n>>> ");

        read_input(cmd);

        printf("\n*********** TESTING READING ***********\n");

        for (int  i = 0; i < cmd->argc; i++) {
            printf("argv at %d is %s\n", i, cmd->argv[i]);
        }
        printf("argc is %d\n", cmd->argc);

        parse_input(cmd, red_cmd, pipe_cmd);

        //multi_pipes();

        //pipes(pipe_cmd);

        for (int  i = 0; i < cmd->argc; i++) {
            if (strcmp(cmd->argv[i], "<") == 0 || strcmp(cmd->argv[i], ">") == 0) {
                redir_counter++;
            }
        }

        if (redir_counter > 0) {
            printf("********** redirect executed ***************\n");
            redirect(cmd, red_cmd);
        } else {
            printf("********** main() parsing... ***************\n");
            exec_input(cmd);      
        }

        cleanup();
    }
    exit(0);
}