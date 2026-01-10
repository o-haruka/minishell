


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(void) {
    // 方法1: bufにNULLを渡して自動でメモリを割り当てる場合
    char *current_dir = getcwd(NULL, 0);
    if (current_dir != NULL) {
        printf("Current directory (動的): %s\n", current_dir);
        printf("パスの長さ: %lu文字\n", strlen(current_dir));
        free(current_dir);  // 忘れずにメモリを解放
    } else {
        perror("getcwd error");
    }

    // 方法2: PATH_MAXを使用
    char buf1[PATH_MAX];
    if (getcwd(buf1, sizeof(buf1)) != NULL) {
        printf("Current directory (PATH_MAX): %s\n", buf1);
    } else {
        perror("getcwd error");
    }

    // 方法3: 固定サイズバッファ（一般的）
    char buf2[1024];
    if (getcwd(buf2, sizeof(buf2)) != NULL) {
        printf("Current directory (1024bytes): %s\n", buf2);
    } else {
        perror("getcwd error");
    }

    return 0;
}

