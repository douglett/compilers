basic commands
--------------
- <id> [A-Za-z][A-Za-z0-9]*  ; standard identifier
- <keyword> ; can't assign
- <string> ".*"  ; standard string
- <val> [id | pop | string]  ; value
- let <id> <string>  ; define.
- add/sub/mul/div/mod <id> <val> ; id->float, val->float, operation, id = result. (if id is pop, push result)
- eq/neq/lt/gt <val> <val> ; returns 1 or 0 <string>
- cat <val> <val> ; standard string concatonation. return <string>
- split <val> <val> <val> ; standard split. val, start, end. return <string> <string>
- pop
- push <val>
- print <val> ... ; print some number of vals, space seperated
- input <id> ; gets line input into id


simple problems
---------------
- https://adriann.github.io/programming_problems.html