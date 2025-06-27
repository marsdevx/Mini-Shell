1. still reacheble meamory leaks
2. fix Makefile
3. test it on the 42 pc
4. norminette


5. tests for memleaks:
"
- cd $PWD hi
- env | sort | grep -v SHLVL | grep -v ^_
"

6. ERROR:
minishell> touch invalid_permission
minishell> chmod -w invalid_permission
minishell> ls >outfile01 >invalid_permission >outfile02
- It can create `outfile01` but not `outfile02`, because of the permission denied flag, but still does it
- Test 89 and others like 98, 101, 104, 116, 118, 119, 120, 127, 130**, it's all because when using redirection with permission denied, command must stop at this point, not continue
- 