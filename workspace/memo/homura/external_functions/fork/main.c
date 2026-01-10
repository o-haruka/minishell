#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
pid_t pid;

// ここでプロセスが2つに分裂する！
pid = fork(); 

if (pid == 0) {
    // 子プロセスだけの世界
    // OSから「0」を返されたので、ここが実行される
    printf("私は子プロセスです。\n");
} else {
    // 親プロセスだけの世界
    // OSから「子のPID」を返されたので、ここが実行される
    printf("私は親プロセスです。子のPIDは %d です。\n", pid);
}
return 0;
}