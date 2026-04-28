#!/bin/bash
# gnl.sh

# コンパイル
cc -Wall -Wextra -Werror -D BUFFER_SIZE=1024 *.c -o get_next_line

# 実行
./get_next_line

# クリーンアップ
rm -f get_next_line

