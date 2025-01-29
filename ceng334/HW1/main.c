#include "parser.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int oldfd[2];
int newfd[2];
void shell(parsed_input* pi) 
{
    if(pi->separator == SEPARATOR_NONE )
    {
        for(int i=0; i < pi->num_inputs; i++)
        {
            pid_t id;
            if(pi->inputs[i].type == INPUT_TYPE_COMMAND)
            {
                if(id = fork())
                {
                    waitpid(id, NULL, 0);
                }
                else
                {
                    execvp(pi->inputs[i].data.cmd.args[0],  pi->inputs[i].data.cmd.args);
                }
            }
            else if(pi->inputs[i].type == INPUT_TYPE_SUBSHELL)
            {
                if(id = fork())
                {
                    waitpid(id, NULL, 0);
                }
                else
                {
                    parsed_input* pi_sub = (parsed_input*) malloc(sizeof(parsed_input));
                    parse_line(pi->inputs->data.subshell, pi_sub);
                    shell(pi_sub);
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
    else if(pi->separator == SEPARATOR_PIPE)
    {
        pid_t childs[pi->num_inputs];
        for(int i=0; i < pi->num_inputs; i++)
        {
            if(pi->inputs[i].type == INPUT_TYPE_COMMAND)
            {
                pipe(newfd);
                if(childs[i] = fork())
                {
                    int status;
                    if(i != 0)
                    {
                        close(oldfd[0]);
                        close(oldfd[1]);
                    }
                    oldfd[0] = newfd[0];
                    oldfd[1] = newfd[1];
                }
                else
                {   
                    if(i>0)
                    {
                        dup2(oldfd[0], 0);
                        close(oldfd[0]);
                        close(oldfd[1]);
                    }
                    if(i<(pi->num_inputs-1))
                    {
                        dup2(newfd[1], 1);
                        close(newfd[0]);
                        close(newfd[1]);
                    }
                    execvp(pi->inputs[i].data.cmd.args[0],  pi->inputs[i].data.cmd.args);
                }
            }
            else if(pi->inputs[i].type == INPUT_TYPE_SUBSHELL)
            {
                parsed_input* pi_sub = (parsed_input*) malloc(sizeof(parsed_input));
                parse_line(pi->inputs[i].data.subshell, pi_sub);
                if(pi_sub->separator == SEPARATOR_PIPE || pi_sub->separator == SEPARATOR_NONE || pi_sub->separator == SEPARATOR_SEQ)
                {
                    pipe(newfd);
                    if(childs[i] = fork())
                    {
                        int status;
                        if(i != 0)
                        {
                            close(oldfd[0]);
                            close(oldfd[1]);
                        }
                        oldfd[0] = newfd[0];
                        oldfd[1] = newfd[1];
                    }
                    else
                    {
                        if(i>0)
                        {
                            dup2(oldfd[0], 0);
                            close(oldfd[0]);
                            close(oldfd[1]);
                        }
                        if(i<(pi->num_inputs-1))
                        {
                            dup2(newfd[1], 1);
                            close(newfd[0]);
                            close(newfd[1]);
                        }
                        shell(pi_sub);
                        exit(EXIT_SUCCESS);
                    }
                }
                else if(pi_sub->separator == SEPARATOR_PARA)
                {
                    pipe(newfd);
                    if(childs[i] = fork())
                    {
                        int status;
                        if(i != 0)
                        {
                            close(oldfd[0]);
                            close(oldfd[1]);
                        }
                        oldfd[0] = newfd[0];
                        oldfd[1] = newfd[1];
                    }
                    else
                    {
                        if(i>0)
                        {
                            dup2(oldfd[0], 0);
                            close(oldfd[0]);
                            close(oldfd[1]);
                        }
                        if(i<(pi->num_inputs-1))
                        {
                            dup2(newfd[1], 1);
                            close(newfd[0]);
                            close(newfd[1]);
                        }
                        pid_t id;
                        if(id = fork())// burdan itibaren subshell
                        {
                            waitpid(id, NULL, 0); 
                        }
                        else
                        {
                            int fd[pi_sub->num_inputs][2];
                            pid_t child2[pi_sub->num_inputs+1];
                            for(int j=0; j < pi_sub->num_inputs; j++)
                            {
                                pipe(fd[j]);
                                if(!(child2[j] = fork()))
                                {
                                    close(fd[j][1]);
                                    dup2(fd[j][0], 0);
                                    close(fd[j][0]);
                                    //exec...   
                                    if(pi_sub->inputs[j].type == INPUT_TYPE_COMMAND)
                                    {
                                        execvp(pi_sub->inputs[j].data.cmd.args[0],  pi_sub->inputs[j].data.cmd.args);
                                    }
                                    else if(pi_sub->inputs[j].type == INPUT_TYPE_PIPELINE)
                                    {
                                        pid_t child3[pi_sub->inputs[j].data.pline.num_commands];
                                        for(int k=0; k < pi_sub->inputs[j].data.pline.num_commands; k++)
                                        {
                                            pipe(newfd);
                                            if(child3[k] = fork())
                                            {
                                                if(k != 0)
                                                {
                                                    close(oldfd[0]);
                                                    close(oldfd[1]);
                                                }
                                                oldfd[0] = newfd[0];
                                                oldfd[1] = newfd[1];
                                            }
                                            else
                                            {   
                                                if(k>0)
                                                {
                                                    dup2(oldfd[0], 0);
                                                    close(oldfd[0]);
                                                    close(oldfd[1]);
                                                }
                                                if(k<(pi_sub->inputs[j].data.pline.num_commands-1))
                                                {
                                                    dup2(newfd[1], 1);
                                                    close(newfd[0]);
                                                    close(newfd[1]);
                                                }
                                                execvp(pi_sub->inputs[j].data.pline.commands[k].args[0],  pi_sub->inputs[j].data.pline.commands[k].args);
                                            }
                                        }
                                        for(int k=0; k < pi_sub->inputs[j].data.pline.num_commands; k++)
                                        {
                                            waitpid(child3[k], NULL, 0);
                                        }
                                        exit(EXIT_SUCCESS);
                                    }
                                }
                                else
                                {
                                    close(fd[j][0]);
                                }
                            }
                            if(!(child2[pi_sub->num_inputs] = fork())) //repeater process
                            {
                                char ch;
                                int x;
                                while((x = read(0, &ch, 1)) > 0)
                                {
                                    for(int j=0; j < pi_sub->num_inputs; j++)
                                    {
                                        write(fd[j][1], &ch, x);
                                    }
                                }
                                for(int j=0; j < pi_sub->num_inputs; j++)
                                {
                                    close(fd[j][1]);
                                }
                                exit(EXIT_SUCCESS);
                            }
                            for(int i=0; i < pi_sub->num_inputs;i++)
                            {
                                close(fd[i][1]);
                            }
                            for(int j=0; j < pi_sub->num_inputs+1; j++)
                            {
                                waitpid(child2[j], NULL, 0);
                            }
                            exit(EXIT_SUCCESS);
                        }
                        exit(EXIT_SUCCESS);
                    }
                    
                }
            }
        }
        for(int i=0; i < pi->num_inputs; i++)
        {
            //printf("waiting for %d\n", childs[i]);
            waitpid(childs[i], NULL, 0);
            //printf("success for %d\n", childs[i]);
        }
    }
    else if(pi->separator == SEPARATOR_SEQ)
    {

        for(int i=0; i < pi->num_inputs; i++)
        {
            if(pi->inputs[i].type == INPUT_TYPE_COMMAND)
            {
                pid_t id;
                if(id = fork())
                {
                    waitpid(id, NULL, 0);
                }
                else
                {
                    execvp(pi->inputs[i].data.cmd.args[0],  pi->inputs[i].data.cmd.args);
                }
            }
            else if(pi->inputs[i].type == INPUT_TYPE_PIPELINE)
            {
                pid_t id;
                if(id = fork())
                {
                    waitpid(id, NULL, 0);
                }
                else
                {
                    pid_t child[pi->inputs[i].data.pline.num_commands];
                    for(int j=0; j < pi->inputs[i].data.pline.num_commands; j++)
                    {
                        pipe(newfd);
                        if(child[j] = fork())
                        {
                            int status;
                            if(j != 0)
                            {
                                close(oldfd[0]);
                                close(oldfd[1]);
                            }
                            oldfd[0] = newfd[0];
                            oldfd[1] = newfd[1];
                        }
                        else
                        {   
                            if(j>0)
                            {
                                dup2(oldfd[0], 0);
                                close(oldfd[0]);
                                close(oldfd[1]);
                            }
                            if(j<(pi->inputs[i].data.pline.num_commands-1))
                            {
                                dup2(newfd[1], 1);
                                close(newfd[0]);
                                close(newfd[1]);
                            }
                            execvp(pi->inputs[i].data.pline.commands[j].args[0],  pi->inputs[i].data.pline.commands[j].args);
                        }
                    }
                    for(int j=0; j < pi->inputs[i].data.pline.num_commands; j++)
                    {
                        waitpid(child[j], NULL, 0);
                    }
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }
    else if(pi->separator == SEPARATOR_PARA)
    {
        pid_t child[pi->num_inputs];
        for(int i=0; i < pi->num_inputs; i++)
        {
            if(pi->inputs[i].type == INPUT_TYPE_COMMAND)
            {
                if(!(child[i] = fork()))
                {
                    execvp(pi->inputs[i].data.cmd.args[0],  pi->inputs[i].data.cmd.args);
                }
            }
            else if(pi->inputs[i].type == INPUT_TYPE_PIPELINE)
            {
                if(!(child[i] = fork()))
                {
                    pid_t child2[pi->inputs[i].data.pline.num_commands];
                    for(int j=0; j < pi->inputs[i].data.pline.num_commands; j++)
                    {
                        pipe(newfd);
                        if(child2[j] = fork())
                        {
                            if(j != 0)
                            {
                                close(oldfd[0]);
                                close(oldfd[1]);
                            }
                            oldfd[0] = newfd[0];
                            oldfd[1] = newfd[1];
                        }
                        else
                        {   
                            if(j>0)
                            {
                                dup2(oldfd[0], 0);
                                close(oldfd[0]);
                                close(oldfd[1]);
                            }
                            if(j<(pi->inputs[i].data.pline.num_commands-1))
                            {
                                dup2(newfd[1], 1);
                                close(newfd[0]);
                                close(newfd[1]);
                            }
                            execvp(pi->inputs[i].data.pline.commands[j].args[0],  pi->inputs[i].data.pline.commands[j].args);
                        }
                    }
                    for(int j=0; j < pi->inputs[i].data.pline.num_commands; j++)
                    {
                        waitpid(child2[j], NULL, 0);
                    }
                    exit(EXIT_SUCCESS);
                }
            }
        }
        for(int i=0; i < pi->num_inputs; i++)
        {
            waitpid(child[i], NULL, 0);
        }
    }
}
void ignoreSIGPIPE() {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGPIPE, &sa, NULL);
}

int main()
{
    ignoreSIGPIPE();
    while(1)
    {
        printf("/> ");
        char *line;
        char* q = "quit";
        line = (char *) malloc(sizeof(char));
        char t;
        int index=0;
        while((t = getchar()) != '\n')
        {
            line = (char *)realloc(line, (index+2)*sizeof(char));
            if(line == NULL) {return -1;}
            line[index++] = t;
        }
        line[index]=0;
        if(!strcmp(line, q))
        {
            return 0;
        }
        parsed_input* pi = (parsed_input*) malloc(sizeof(parsed_input));
        parse_line(line, pi);
        shell(pi);
    }
}