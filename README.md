# VPML - Personal Markup Language


https://vbprojects.github.io/vpml/

This is a small project meant to make a parser and transpiler/preprocessor from my personal markup language (vpml) to html. The first implementation in VPMLR is written in base julia. The second implementation is written in C99 with only the standard library.

vpml is a pythonic-esque way of writing html meant for use with css frameworks like tailwind. What differentiates this from tools like pug, haml, and ember is that the syntax assumes in most cases you are specifying classes. For Example,

```
head
  title
    VPML - Example
  link rel="stylesheet" href="styles.css"
html
? this is a comment
    body #body bg-black
        div #content bg-gray-800 #1
            h1 bg-blue
                Hello World
            p
?           this is a comment
                raw This is my site
        div #content bg-gray-800 #2
            h1 bg-blue
                Hello World

```

If the first token of the line is not a valid html tag then the line is considered text. For lines that start with tags, any token that starts with # is considered an id. Any token that has an '=' char in it is considered an attribute. Otherwise, the token is considered a class. Tokens are delimited by ' '. Lines that are all whitespace characters or are empty are ignored. Lines that start with the char '?' are ignored.

<!-- vpc.c has a total non comment lines equal to 216 and an average cyclomatic complexity of 4 and a max of 10. -->
<!-- gpp (general preprocessor) -->


```bash
clang -O3 vpc.c -o bin/vpc.exe
clang -O3 gpp.c -o bin/gpp.exe

gpp index.vpml -o index.vpmlp && vpc index.vpmlp index.html
```

You can build both with whatever C99 compiler you want.
