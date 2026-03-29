# minishell

## 開発ルール

### gitのルール

- コードを触り始める時必ずpull（mainでpullして、自分のプランチにmergeする）
- 基本的には、自分のブランチで作業する
- コードを一旦書き終えたタイミングで必ずpushする（こまめにpush！）+ pullリク出す
- gitを破壊してしまったらすぐに申告（慌てない、自分で何とかしようとしない）
- （初心者なので、）余裕があれば、バックアップを取っておく。

### freeのタイミング

・作った側でなく、使い終わった側がfreeする


### エラーメッセージ
・ 具体的なエラーメッセージは検討中。文言は保留 (3/29 MTG)
・ 誰がエラーを出力するかのルール： 「自分の担当フェーズで発見したエラーは自分が出力する」
・ エラー後の処理方法： 保留。 Claude 案： 「エラーを発見した関数は NULL または 1 を返す。main ループ側がそれを受け取って last_status = 2 をセットし、Executorには渡さずに次のループへ continue する」


### ディレクトリー構成案 (claude 2026/3/29)
```
minishell/
├── Makefile
├── README.md
├── includes/
│   └── minishell.h          ← 構造体・プロトタイプ・define を全部ここに
├── libft/
│   ├── Makefile
│   ├── includes/
│   │   └── libft.h
│   └── srcs/
│       └── *.c
└── srcs/
    ├── main.c
    ├── lexer/
    │   ├── lexer.c          ← トークン分割メインループ
    │   ├── lexer_utils.c    ← 補助関数
    │   ├── quote.c          ← クォート処理
    │   └── token.c          ← トークンの生成・管理
    ├── parser/
    │   ├── parser.c         ← トークン列 → コマンドリスト変換
    │   ├── parser_utils.c
    │   └── expand.c         ← $VAR / $? の展開
    ├── executor/
    │   ├── executor.c       ← 実行エンジン本体（fork/execve）
    │   ├── executor_utils.c
    │   ├── pipe.c           ← パイプライン処理
    │   ├── redirect.c       ← リダイレクト処理（dup2）
    │   └── path.c           ← PATH 検索
    ├── builtins/
    │   ├── builtin_echo.c   ← echo (-n)
    │   ├── builtin_cd.c     ← cd
    │   ├── builtin_pwd.c    ← pwd
    │   ├── builtin_env.c    ← env
    │   ├── builtin_export.c ← export
    │   ├── builtin_unset.c  ← unset
    │   └── builtin_exit.c   ← exit
    ├── env/
    │   ├── env_init.c       ← 環境変数リストの初期化
    │   └── env_utils.c      ← 環境変数の検索・更新
    ├── signal/
    │   └── signal.c         ← ctrl-C / ctrl-D / ctrl-\ 処理
    └── utils/
        ├── error.c          ← エラー出力・終了処理
        ├── free.c           ← メモリ解放まとめ
        └── utils.c          ← 汎用補助関数
```





## 📋 使用可能外部関数一覧（カテゴリ別）

### 📖 入力・履歴管理

- `readline` - ユーザーからの行入力取得
- `rl_clear_history` - 履歴クリア
- `rl_on_new_line` - 新しい行の通知
- `rl_replace_line` - 現在の行を置換
- `rl_redisplay` - 行の再表示
- `add_history` - 履歴に追加

### 💾 基本I/O操作

- `printf` - フォーマット出力
- `write` - ファイル記述子への書き込み
- `read` - ファイル記述子からの読み込み

### 🧠 メモリ管理

- `malloc` - メモリ確保
- `free` - メモリ解放

### 📁 ファイル操作

- `access` - ファイルアクセス権確認
- `open` - ファイルオープン
- `close` - ファイルクローズ
- `unlink` - ファイル削除

### 🔄 プロセス制御

- `fork` - 子プロセス作成
- `wait` - 子プロセス終了待ち
- `waitpid` - 特定プロセス終了待ち
- `wait3` - プロセス終了待ち（リソース情報付き）
- `wait4` - 特定プロセス終了待ち（リソース情報付き）
- `exit` - プロセス終了
- `execve` - プログラム実行

### ⚡ シグナル処理

- `signal` - シグナルハンドラ設定
- `sigaction` - シグナルアクション設定
- `sigemptyset` - シグナルセット初期化
- `sigaddset` - シグナルセットに追加
- `kill` - シグナル送信

### 📂 ディレクトリ操作

- `getcwd` - 現在のディレクトリ取得
- `chdir` - ディレクトリ変更
- `opendir` - ディレクトリオープン
- `readdir` - ディレクトリエントリ読み取り
- `closedir` - ディレクトリクローズ

### 📊 ファイル情報取得

- `stat` - ファイル情報取得
- `lstat` - シンボリックリンク情報取得
- `fstat` - ファイル記述子の情報取得

### 🔗 パイプ・リダイレクト

- `dup` - ファイル記述子複製
- `dup2` - ファイル記述子複製（指定先）
- `pipe` - パイプ作成

### 🌍 環境変数

- `getenv` - 環境変数取得

### ❌ エラー処理

- `strerror` - エラー番号から文字列取得
- `perror` - エラーメッセージ出力

### 🖥️ 端末制御

- `isatty` - 端末かどうか確認
- `ttyname` - 端末名取得
- `ttyslot` - 端末スロット取得
- `ioctl` - デバイス制御
- `tcsetattr` - 端末属性設定
- `tcgetattr` - 端末属性取得

### 📺 Termcap（端末機能）

- `tgetent` - 端末データベース読み込み
- `tgetflag` - 真偽値機能取得
- `tgetnum` - 数値機能取得
- `tgetstr` - 文字列機能取得
- `tgoto` - カーソル移動文字列生成
- `tputs` - 端末制御文字列出力
