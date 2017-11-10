#!/root/projects/OS-2017/p_set3/mysh
#This is an example of a shell script that your shell must execute, notice that lines starting with # sign are ignored as comments!
#Let's say this here file is called testme.sh. You created it with vi testme.sh ; chmod +x testme.sh# You invoked it with ./testme.sh
cat >cat.out
# at this point, type some lines at the keyboard, then create an EOF
#your shell invoked the system cat command with output redirected to cat.out
cat cat.out
#you better see the lines that you just typed!
exit 123
#after your script exits, type echo $? from the UNIX system shell, the value should be 123. Since your shell just exited, the following bogus command should never be seen
