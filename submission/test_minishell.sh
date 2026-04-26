#!/bin/bash
# =============================================================================
# test_minishell.sh
# minishell と bash の挙動を比較するテストスクリプト
# ※ && と ; は minishell の必須要件外のため除外
# 使い方: bash test_minishell.sh
# =============================================================================

# --- 色の定義 ---
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
CYAN="\033[36m"
BOLD="\033[1m"
RESET="\033[0m"

# --- Minishell の実行ファイルパス ---
MINISHELL="./minishell"

# --- 一時ファイル ---
TMPDIR_WORK=$(mktemp -d)
BASH_OUT="$TMPDIR_WORK/bash_out"
BASH_ERR="$TMPDIR_WORK/bash_err"
BASH_STATUS="$TMPDIR_WORK/bash_status"
MINI_OUT="$TMPDIR_WORK/mini_out"
MINI_ERR="$TMPDIR_WORK/mini_err"
MINI_STATUS="$TMPDIR_WORK/mini_status"
BASH_ERR_CLEAN="$TMPDIR_WORK/bash_err_clean"
MINI_ERR_CLEAN="$TMPDIR_WORK/mini_err_clean"

# --- 統計 ---
TOTAL=0
SUCCESS=0
CURRENT_SECTION=""

# =============================================================================
# テストケースの定義
# 形式: "セクション名::コマンド文字列"
# =============================================================================
TEST_CASES=(
    # --- echo ---
    "echo::echo hello world"
    "echo::echo -n no newline"
    "echo::echo "
    "echo::echo \$?"

    # --- pwd ---
    "pwd::pwd"

    # --- cd ---
    "cd::cd /tmp"
    "cd::cd"
    "cd::cd nonexistent_dir_xyz"

    # --- env ---
    "env::env | grep ^HOME"
    "env::env | grep ^PATH"

    # --- export ---
    "export::export NOVALUE"
    "export::export _T42=hello"

    # --- unset ---
    "unset::unset PATH"
    "unset::unset UNDEFINED_VAR_XYZ"

    # --- exit ---
    "exit::exit 0"
    "exit::exit 42"
    "exit::exit 256"
    "exit::exit abc"

    # --- パイプ ---
    "pipe::ls | grep a"
    "pipe::echo hello | cat"
    "pipe::cat /etc/hosts | grep localhost | head -1"
    "pipe::echo hello | cat | cat"

    # --- リダイレクト: 出力 ---
    "redirect::echo hello > /tmp/ms_test_out.txt"
    "redirect::echo hello >> /tmp/ms_test_app.txt"

    # --- リダイレクト: 入力 ---
    "redirect::cat < /etc/hosts | head -1"

    # --- 環境変数展開 ---
    "expand::echo \$HOME"
    "expand::echo \$USER"
    "expand::echo \$UNDEFINED_VAR_XYZ"
    "expand::echo \$?"

    # --- クォート ---
    "quote::echo 'single \$HOME'"
    "quote::echo \"double \$HOME\""
    "quote::echo \"  spaces  \""
    "quote::echo ''"

    # --- エラーケース ---
    "error::cat does_not_exist_xyz"
    "error::nonexistent_command_xyz"
    "error::ls /nonexistent_path_xyz"
    "error::cd nonexistent_dir_xyz"
)

# =============================================================================
# エラー出力の正規化
#   bash  : "bash: line 1: foo: command not found" -> "foo: command not found"
#   mini  : "minishell: foo: command not found"    -> "foo: command not found"
# =============================================================================
normalize_err() {
    local infile="$1"
    local outfile="$2"
    sed 's/^bash: line [0-9]*: //' "$infile" \
        | sed 's/^bash: //' \
        | sed 's/^minishell: //' > "$outfile"
}

# =============================================================================
# セクション見出しを出力する
# =============================================================================
print_section() {
    echo ""
    echo -e "${CYAN}${BOLD}[ $1 ]${RESET}"
    echo "------------------------------------------------------------"
}

# =============================================================================
# メインループ
# =============================================================================
echo -e "${BOLD}🚀 Starting Minishell vs Bash Tests...${RESET}"

if [ ! -x "$MINISHELL" ]; then
    echo -e "${RED}ERROR: $MINISHELL が見つからないか実行権限がありません${RESET}"
    exit 1
fi

for entry in "${TEST_CASES[@]}"; do
    # "セクション::コマンド" を分割
    SECTION="${entry%%::*}"
    CMD="${entry#*::}"

    # セクションが変わったら見出しを表示
    if [ "$SECTION" != "$CURRENT_SECTION" ]; then
        print_section "$SECTION"
        CURRENT_SECTION="$SECTION"
    fi

    ((TOTAL++))

    # --- bash で実行 ---
    bash -c "$CMD" > "$BASH_OUT" 2> "$BASH_ERR"
    echo $? > "$BASH_STATUS"

    # --- minishell で実行 ---
    echo "$CMD" | env TERM=xterm "$MINISHELL" > "$MINI_OUT" 2> "$MINI_ERR"
    echo $? > "$MINI_STATUS"

    # --- エラー出力を正規化 ---
    normalize_err "$BASH_ERR" "$BASH_ERR_CLEAN"
    normalize_err "$MINI_ERR" "$MINI_ERR_CLEAN"

    # --- 比較 ---
    diff -q "$BASH_OUT"        "$MINI_OUT"        > /dev/null 2>&1; OUT_DIFF=$?
    diff -q "$BASH_STATUS"     "$MINI_STATUS"     > /dev/null 2>&1; STATUS_DIFF=$?
    diff -q "$BASH_ERR_CLEAN"  "$MINI_ERR_CLEAN"  > /dev/null 2>&1; ERR_DIFF=$?

    # --- 結果表示 ---
    if [ $OUT_DIFF -eq 0 ] && [ $STATUS_DIFF -eq 0 ] && [ $ERR_DIFF -eq 0 ]; then
        echo -e "  ${GREEN}[OK]${RESET}  $CMD"
        ((SUCCESS++))
    else
        echo -e "  ${RED}[KO]${RESET}  $CMD"
        if [ $OUT_DIFF -ne 0 ]; then
            echo -e "    ${YELLOW}stdout differ${RESET}"
            echo "      bash : $(head -3 "$BASH_OUT")"
            echo "      mini : $(head -3 "$MINI_OUT")"
        fi
        if [ $STATUS_DIFF -ne 0 ]; then
            BASH_S=$(cat "$BASH_STATUS")
            MINI_S=$(cat "$MINI_STATUS")
            echo -e "    ${YELLOW}exit status : bash=$BASH_S  mini=$MINI_S${RESET}"
        fi
        if [ $ERR_DIFF -ne 0 ]; then
            echo -e "    ${YELLOW}stderr differ${RESET}"
            echo "      bash : $(head -3 "$BASH_ERR_CLEAN")"
            echo "      mini : $(head -3 "$MINI_ERR_CLEAN")"
        fi
    fi
done

# --- 後片付け ---
rm -rf "$TMPDIR_WORK"
rm -f /tmp/ms_test_out.txt /tmp/ms_test_app.txt

# --- 最終結果 ---
echo ""
echo "=================================================="
FAIL=$((TOTAL - SUCCESS))
echo -e "${BOLD}Result: ${GREEN}$SUCCESS passed${RESET} / ${RED}$FAIL failed${RESET} / $TOTAL total"
echo "=================================================="

# CI 用: 全部 OK なら 0、1つでも KO なら 1
[ $SUCCESS -eq $TOTAL ] && exit 0 || exit 1
