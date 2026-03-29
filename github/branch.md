# mainの内容を自分のブランチに取り組む方法

### ①mainブランチに移動
git checkout main

### ②リモートのmainを取得
git pull origin main

### ③homuraブランチに移動
git checkout homura

### ④mainの内容をhomuraに取り込む
git merge main

### ⑤リモートのhomuraに反映
git push origin homura


--------------------------------------

# 自分のブランチの確認方法

`git branch`


# ブランチの移動方法

`git branch <移動したいブランチ>`
