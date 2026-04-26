_This project has been created as part of the 42 curriculum by hkuninag, homura._

## Description

## Instructions

### Compilation

```bash
make
```

### Execution & Leak Check

```bash
**valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-fds=yes \
         --trace-children=yes \
         --suppressions=readline.supp \
         ./minishell**
```

- `--leak-check=full` & `--show-leak-kinds=all`  
  基本中の基本です。到達不能なメモリ（definitely lost）だけでなく、間接的なリーク（indirectly lost）などもすべて詳細に出力します。

- `--track-fds=yes`（超重要！）  
  **ファイルディスクリプタの閉じ忘れ（FDリーク）**を監視します。Minishellでは `pipe()` や `dup2()`、リダイレクト（`<`, `>`）で大量のFDを扱います。FDを閉じ忘れると、パイプラインがハングアップしたり、長時間起動しているとシェルがクラッシュしたりします。このオプションは絶対に付けてください。

- `--trace-children=yes`（超重要！）  
  子プロセスも追跡対象にします。Minishellは外部コマンドを実行する際やパイプを繋ぐ際に `fork()` を行います。このオプションがないと、**「forkした先の子プロセスで起きたメモリリーク」**を検知できません。

- `--suppressions=readline.supp`  
  `readline` 関数由来の「課題として許容されているメモリリーク」の警告をミュート（非表示）にするための設定ファイルを読み込ませます。

## Resources
