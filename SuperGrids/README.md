###############################################################
create a new repository on the command line

echo "# SuperGrids" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/beolee/SuperGrids.git
git push -u origin master
###############################################################
push an existing repository from the command line

git remote add origin https://github.com/beolee/SuperGrids.git
git push -u origin master
###############################################################
git add SuperGrids/SuperGrids/*.h
git add SuperGrids/SuperGrids/*.cpp
git add SuperGrids/SuperGrids/*.lib
git add SuperGrids/SuperGrids/*.dll
git add SuperGrids/SuperGrids/*.ico
git add SuperGrids/SuperGrids/*.xml
git add SuperGrids/SuperGrids/*.dtd
git add SuperGrids/SuperGrids/*.txt
git add SuperGrids/Release/*.exe
git commit -m "add some nessary files"
git remote add https://github.com/beolee/SuperGrids.git
git push origin master

###############################################################
git pull命令的作用是，取回远程主机某个分支的更新，再与本地的指定分支合并。它的完整格式稍稍有点复杂。
$ git pull <远程主机名> <远程分支名>:<本地分支名>
如果远程分支是与当前分支合并，则冒号后面的部分可以省略。
git pull origin master