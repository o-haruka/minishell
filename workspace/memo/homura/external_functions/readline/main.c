#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

int main(void) {
  char *line = readline("入力してください: ");
  if (line) {
    printf("入力内容: %s\n", line);
    free(line); // 必ず解放する
  } else {
    printf("入力がありませんでした。\n");
  }
  return 0;
}