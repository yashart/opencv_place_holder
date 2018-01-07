#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include "send_img_ssh.h"


void fork_and_exec_scp(char* img, char* scpParam) {
    if (fork() == 0) {
        //execlp("/bin/echo", "" , "hi!", NULL);
        execlp("/usr/bin/scp", "", img , scpParam, NULL);
    }
}
