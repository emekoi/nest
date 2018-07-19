mkdir -p wren && cd wren
git init
git remote add -ft master origin https://github.com/munificent/wren.git
git config core.sparsecheckout true
echo src/include >> .git/info/sparse-checkout
echo src/vm >> .git/info/sparse-checkout
echo src/optional >> .git/info/sparse-checkout
echo /LICENSE >> .git/info/sparse-checkout
git pull origin master
