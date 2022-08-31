#tp - A Better Type  
Fred Theilig  

Way back in my college days we programmed on a Prime minicomputer. I had little experience with or access to anything Unix-like and the Prime was rather
poor when it came to text file manipulation. I wrote tp to fill a need. Much of the functionality is rather pointless today but I hope to update it
to be more GNU-like and maybe even useful.  
The options are as follows:
```
   -h   Display a hexadecimal listing of the file
   -o   Display an Octal listing of the file
        -h takes prescidence if both -h and -o are selected
   -n   Display line numbers
   -b   Display filename at begining of text
   -f   Display formfeed at end of text
   -p   Prompt after every page
        All options can be toggled at the more prompt, and you can enter 'q' to quit
   -x   Filter non-printable characters
   -t   Show only tail (last page) of text
   -d   Remove tab characters
   -?   Display this text and exit
```
The -p prompt option is fairly pointless except that options can be toggled. Most people will want to pipe through
less or more. I will add additional options as I see fit. Maybe include Linux tac and strings functionality.
