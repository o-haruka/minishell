# mainの内容を自分のブランチに取り組む方法

### ①mainブランチに移動

git checkout main

### ②リモートのmainを取得

git pull
(このコマンドで機能するように、事前に git branch --set-upstream-to=origin/main main コマンドで設定しておく)

### ③homuraブランチに移動

git checkout homura(/hkuninag)

### ④mainの内容をhomuraに取り込む

git merge main

### ⑤リモートのhomuraに反映

git push origin homura(/hkuninag)

---

# 自分のブランチの確認方法

`git branch`

# ブランチの移動方法

`git branch <移動したいブランチ>`

# 🧹 PRマージ後の完璧なお掃除手順

## **1. mainブランチに戻って、最新状態にする**

```bash
git checkout main
git pull origin main
```

## **2. GitHub（リモート）上のブランチを削除する**

```bash
git push origin --delete 削除したいブランチ名
```

_(※GitHubのWeb画面のボタンで削除した場合は、このコマンドは不要)_

## **3. GitHubで消えた情報を、手元のGitにも反映（同期）させる**

```bash
git fetch --prune
```

リモートのブランチの状況を反映

## **4. 手元のPC（ローカル）に残っているブランチも削除する**

```bash
git branch -d 削除したいブランチ名
```
