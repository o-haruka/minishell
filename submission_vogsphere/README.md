_This project has been created as part of the 42 curriculum by hkuninag, homura._

## Description

**minishell** is a simplified Unix shell implemented in C, built as part of the 42 school curriculum. It is modeled after `bash`.

We built a fully interactive command-line interpreter, gaining hands-on knowledge of how shells work — including how processes are created, how file descriptors are managed, how pipes connect programs, and how signals interrupt execution.

### Features

- Interactive prompt with command history (via `readline`)
- PATH-based executable search and launch
- Single and double quote handling
- Environment variable expansion (`$VAR`, `$?`)
- Input/output redirections: `<`, `>`, `<<` (here-doc), `>>`
- Pipes (`|`) connecting command output to the next command's input
- Signal handling: `ctrl-C` (new prompt), `ctrl-D` (exit), `ctrl-\` (ignored)
- Built-in commands:
  - `echo` (with `-n` option)
  - `cd` (relative and absolute paths)
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`


### Project Structure

```
minishell/
├── Makefile
├── includes/
│   └── minishell.h              # shared structs and prototypes
├── libft/                       # custom C library
└── srcs/
    ├── main.c
    ├── signal.c
    ├── terminal.c               # terminal settings
    ├── utils.c                  # shared utility functions
    ├── tokenizer/               # lexical analysis (input → tokens)
    │   ├── tokenizer.c
    │   ├── tokenizer_syntax.c
    │   ├── tokenizer_utils.c
    │   ├── token_operator.c
    │   ├── token_word.c
    │   └── token.c
    ├── parser/                  # syntax analysis + variable expansion
    │   ├── parser.c
    │   ├── parser_redirect.c
    │   ├── parser_utils.c
    │   ├── expand.c
    │   └── expand_utils.c
    ├── env/                     # environment variable list management
    │   ├── env_init.c
    │   ├── env_update.c
    │   └── env_utils.c
    ├── executor/                # fork/exec, pipes, redirections
    │   ├── executor.c
    │   ├── execute_builtin.c
    │   ├── path.c
    │   ├── pipe.c
    │   ├── pipe_utils.c
    │   ├── redirect.c
    │   └── redirect_heredoc.c
    └── builtins/                # built-in command implementations
        ├── builtin_cd.c
        ├── builtin_echo.c
        ├── builtin_env.c
        ├── builtin_exit.c
        ├── builtin_export.c
        ├── builtin_pwd.c
        ├── builtin_unset.c
        └── builtin_utils.c
```



```
=============================================================================
                     Minishell Execution Lifecycle
=============================================================================

[ User keyboard input ]
       |
       v
=============================================================================
[1. Readline]                                    main.c  read_line_input()
       |  Reads one line of input from the user and allocates it as a string.
       |  line = "ls -l | grep .c > out.txt"
       v
-----------------------------------------------------------------------------
[2. Lexer (tokenizer)]                       tokenizer.c  tokenize()
       |  Splits the input string into a linked list of meaningful units
       |  (tokens). Handles whitespace skipping and classifies each token
       |  as an operator or a word.
       |  Note: quoted strings are kept as-is here;
       |        variable expansion is delegated to the Expander (step 4).
       v
  [t_token]         [t_token]        [t_token]        [t_token]
 +----------+      +----------+     +----------+     +-----------+
 |TK_WORD   | -->  |TK_WORD   | --> |TK_PIPE   | --> |TK_WORD    | --> ...
 |word: "ls"|      |word: "-l"|     |word: "|" |     |word: "grep"|
 +----------+      +----------+     +----------+     +-----------+
                                                        ... --> [TK_EOF]
       |
       v
-----------------------------------------------------------------------------
[2.5. Syntax Check]            tokenizer_syntax.c  ft_check_syntax()
       |  Catches syntax errors that can be detected before execution:
       |  consecutive pipes (e.g. ||), trailing pipe, invalid redirection
       |  operators, etc.
       |  On error: writes a message to STDERR and returns NULL to abort.
       v
-----------------------------------------------------------------------------
[3. Parser]              parser.c / parser_redirect.c  parse()
       |  Groups the token list into t_cmd structs, split on TK_PIPE
       |  boundaries. Redirection info (t_redir) is extracted and attached
       |  to each t_cmd by handle_redirection().
       v
  [t_cmd  (cmd 1)]                            [t_cmd  (cmd 2)]
 +---------------------------+    next        +---------------------------+
 | args: ["ls", "-l", NULL]  | ------------> | args: ["grep", ".c", NULL]|
 | redirs: NULL              |               | redirs: --> [t_redir]     |
 +---------------------------+               +---------------------------+
                                                           |
                                                           v
                                                      [t_redir]
                                                 +----------------------+
                                                 | kind: TK_REDIRECT_OUT|
                                                 | file: "out.txt"      |
                                                 +----------------------+
       |
       v
-----------------------------------------------------------------------------
[4. Expander]                    expand.c / expand_utils.c
       |                                       ft_expand_args()
       |  Walks every args[] and redir->file in the t_cmd list and replaces
       |  environment variable references with their actual values,
       |  looked up from t_shell.env.
       |    $VAR  -->  value from env list
       |    $?    -->  value of shell->last_status
       |  Single-quoted strings: not expanded.
       |  Double-quoted strings: only $ inside is expanded.
       v
-----------------------------------------------------------------------------
[5. Executor]           executor.c / execute_builtin.c  ft_execute()
       |  Determines whether the command is a built-in (echo, cd, export,
       |  etc.) or an external command, and routes accordingly.
       |  For pipelines, ft_execute_pipeline() manages the full pipeline.
       v

  (parent process: minishell)
        |
        |  [built-in command]
        +-----------------> execute_builtin() called directly in the parent
        |                   (no fork -- changes to env/cwd are reflected
        |                    in the parent shell)
        |
        |  [external command]
        |  open_all_pipes()  -- allocates pipe fds for each "|" boundary
        |
        +-- fork() --> [child 1]
        |               set_signal_for_child()
        |               ft_apply_redirs()     -- redirect stdout to pipe write-end
        |               execve("/bin/ls", ["ls", "-l"], envp)
        |
        |                         (kernel pipe buffer)
        |
        +-- fork() --> [child 2]
        |               set_signal_for_child()
        |               ft_apply_redirs()     -- redirect stdin from pipe read-end
        |                                    -- redirect stdout to "out.txt" (dup2)
        |               execve("/usr/bin/grep", ["grep", ".c"], envp)
        |
        |  set_signal_for_parent_wait()  -- parent ignores signals while waiting
        |  wait_all_cmds()               -- waitpid() for every child;
        |                                   collects exit status into last_status
        v
-----------------------------------------------------------------------------
[6. Cleanup]                                   main.c  process_command()
       |  At the end of each command execution, all allocated memory is freed
       |  to prevent memory leaks.
       |    free_cmds_list(shell->cmds)  -- frees t_cmd + t_redir + args[]
       |    token_free(&shell->tokens)   -- frees t_token linked list
       |    free(line)                   -- frees the readline buffer
       |
       |  On shell exit (ctrl-D / exit builtin):
       |    free_env(shell->env)         -- frees t_env linked list
       |    rl_clear_history()           -- frees readline history
       v
[ Loop back to [1. Readline] ]
=============================================================================
```

## Instructions


### zsh to bash

**Switch from zsh (the school PC default) to bash, and confirm the current shell:**

```bash
bash
echo $0
```

The output will be something like `-bash` or `-zsh`.

> `echo $0` is the simplest and most reliable way to check the active shell.
> `echo $SHELL` only shows the **login-time default**, so it may differ from the shell you are actually running if you switched mid-session.



### Compilation

```bash
make
```

This compiles `libft` and then the project, producing the `minishell` executable.

To clean object files:

```bash
make clean
```

To remove all compiled files including the binary:

```bash
make fclean
```

To recompile from scratch:

```bash
make re
```


### Execution

```bash
./minishell
```

You will see a prompt. Type commands just like in `bash`.


### Usage Examples

```bash
minishell$ echo "Hello, $USER"
Hello, yourname

minishell$ ls -la | grep minishell
-rwxr-xr-x  1 user  staff  12345 Apr 28 10:00 minishell

minishell$ export GREETING=hello
minishell$ echo $GREETING
hello

minishell$ cat << EOF
> line1
> line2
> EOF
line1
line2

minishell$ exit
```

---
### Leak Check

Use `valgrind` to detect memory leaks and file descriptor leaks:

```bash
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-fds=yes \
         --trace-children=yes \
         --suppressions=readline.supp \
         ./minishell
```

- `--leak-check=full` & `--show-leak-kinds=all`
  The basics. Reports all categories of leaks in detail — not just unreachable memory (`definitely lost`) but also indirect leaks (`indirectly lost`) and more.
- `--track-fds=yes` **critical**
  Detects **file descriptor leaks** (forgetting to `close()` an FD). minishell opens a large number of FDs via `pipe()`, `dup2()`, and redirections (`<`, `>`). An unclosed FD can cause pipelines to hang or the shell to crash after extended use. Always include this option.
- `--trace-children=yes` **critical**
  Extends tracking to child processes. minishell calls `fork()` when executing external commands and building pipelines. Without this option, memory leaks that occur **inside forked child processes** will go undetected.
- `--suppressions=readline.supp`
  Loads a suppression file to silence warnings from `readline`-internal leaks that are explicitly permitted by the project specification.


## Resources

### References

- **The Linux Programming Interface** (Michael Kerrisk, No Starch Press)
  Used as the main reference for system calls. Key chapters:
  - Ch. 5 — File I/O (`dup`, `dup2`)
  - Ch. 6 — Environment variables
  - Ch. 20–22 — Signals and `sigaction`
  - Ch. 24–26 — Processes, `fork`, `execve`, `waitpid`
  - Ch. 44 — Pipes
- **POSIX Shell & Utilities** — https://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html


### How AI Was Used

AI tools (Claude, Gemini, and ChatGPT) were used throughout this project as a learning and productivity aid, strictly following the 42 AI Instructions guidelines. All AI-generated content was reviewed, understood, and validated by both team members before use.

AI assisted with data structure design (`t_shell`, `t_cmd`, `t_token`, `t_env`), explaining Unix concepts such as pipe internals, `dup2`, `fork`/`exec` flow, and signal handling with `sigaction`. It also helped identify how to split functions into Norm-compliant files, diagnose bugs (e.g., `static` linkage across files, quote-state tracking for `$` expansion), and draft this README.
