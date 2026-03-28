## git pushだけでpushする

git pushコマンドでpushしても反映されない問題があった。

### 何が問題だったか

- ローカルの`main`には、`gemini_inst.md`を含むコミット（`21d6158`）が存在していた。
- ただし当初、`main`にupstream（追跡先）が設定されていなかった（`NO_UPSTREAM`）。
- さらに、リモート確認時にSSH認証で失敗していた（`Permission denied (publickey)`）。

つまり、**「コミットがない」のではなく、`origin/main`へ正常にpushできていなかった**のが原因。

### 実際に行った対応

- `git push -u origin main` を実行。
- SSHパスフレーズ入力後、push成功：`main -> main`。
- 同時に`main`のupstreamも設定された。

### 今後の確認ポイント

1. `git status -sb` で現在ブランチ確認
2. `git branch -vv` でupstream有無確認
3. `git push -u origin main`（初回のみ`-u`）
4. SSHエラーが出たら、鍵・パスフレーズ入力を再確認
