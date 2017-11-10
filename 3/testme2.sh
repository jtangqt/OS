#!/root/projects/OS-2017/p_set3/mysh
#This is an example of a shell script that your shell must execute correction
cat >cat2.out
#since you invoked the shell script (via the system shell such as bash)
#with stdin redirected, your shell runs cat which gets stdin from input.txt
exit
#above exit has no specified return value, so your shell exited with 0