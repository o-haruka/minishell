# branchの消し方

## GitHub（リモート）上のブランチを削除する

```bash
git push origin --delete 削除したいブランチ名
```

_(※GitHubのWeb画面のボタンで削除可能)_

## GitHubで消えた情報を、手元のGitにも反映（同期）させる\*\*

```bash
git fetch --prune
```

リモートのブランチの状況を反映

## 手元のPC（ローカル）に残っているブランチも削除する\*\*

```bash
git branch -d 削除したいブランチ名
```

`git branch`で消えたか確認する。
