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


--------------------------------------

# 自分のブランチの確認方法

`git branch`


# ブランチの移動方法

`git branch <移動したいブランチ>`
