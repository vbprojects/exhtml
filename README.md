# Exhtml - Personal Markup Language


https://vbprojects.github.io/vpml/

This is a small project meant to make a parser and transpiler/preprocessor from my personal markup language (vpml) to html. The first implementation in VPMLR is written in base julia. The second implementation is written in C99 with only the standard library. The third is implemented in JavaScript. The C99 version is the most basic but smallest and fastest requiring only the standard library. The JavaScript version consists of vpj.js, which exposes a method process_string for compiling js strings into html and vpnj.js, which requires node for file io. vpnj.js can be compiled to a single executable with node js.

vpml is a pythonic-esque way of writing html meant for use with css frameworks like tailwind. What differentiates this from tools like pug, haml, and ember is that the syntax assumes in most cases you are specifying classes. For Example,

```
head
  title
    VPML - Example
  link rel="stylesheet" href="styles.css"
html
# this is a comment
    body #body bg-black
        div #content bg-gray-800 #1
            h1 bg-blue
                Hello World
            p
#           this is a comment
                raw This is my site
        div #content bg-gray-800 #2
            h1 bg-blue
                Hello World

```

If the first token of the line is not a valid html tag then the line is considered text. For lines that start with tags, any token that starts with # is considered an id. Any token that has an '=' char in it is considered an attribute. Otherwise, the token is considered a class. Tokens are delimited by ' '. Lines that are all whitespace characters or are empty are ignored. Lines that start with the char '#' are ignored.

<!-- vpc.c has a total non comment lines equal to 216 and an average cyclomatic complexity of 4 and a max of 10. -->
<!-- gpp (general preprocessor) -->


```bash
clang -O3 vpc.c -o bin/vpc.exe
clang -O3 gpp.c -o bin/gpp.exe

gpp index.vpml -o index.vpmlp && vpc index.vpmlp index.html
```

You can build both with whatever C99 compiler you want. The reason the JS version exists is because it seems like even the most space efficient executables for compiling vpc to wasm are still nearly 36kb, the JS version minified is around 4.9kb.

The JS compiler includes a built in macro system which allows for text substitution in leaf elements of strings, and additional syntax for specifying the text of elements.

Macros can be defined by starting a line as such `:macro !<name_of_macro>`
The text of an element can be defined as follows `tag [inside first tag] | <text>`

Macro names must start with ! and must be invoked by starting a line with `!<macro_name>`

```
:macro !hello_world
  div
    h1 | @0
    h2 | @1
!helllo_world
  Hello World!
  div useless_class #useless_id | Also Hello World!
```

```
div
  h1
    Hello World!
  h2
    Also Hello World!
```

Macros are only available to lines processed after their declaration. Raw strings are not tokenized and thus cannot have macro string interpolation

```
:macro !not_hello_world
  div
    h1
      raw @0
!not_hello_world
  Hello World!
```

```
div
  h1
    raw @0
```
