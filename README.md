# trie-builder
Builds a prefix string that lets you identify all lines in the file uniquely by
checking only the characters specified in the specified position, as well as the
length of the line.

The program can be made using make. I have tested it on Ubuntu 22.04 and macOS
Sonoma 14.5. The program requires unix line endings. I don't think I will be
maintaining or improving this any further, since I just made it to improve the
speed of parsing some lines in a parser.

### Usage
```text
usage: ./buildtrie [SOURCE_FILE]
SOURCE_FILE: File that contains lines that should be converted to a Trie.
             The maximum length of the name is 200 characters.
```

### Example
Example `dummy.txt` file:
```text
Accept-Patch
Accept-Ranges
Age
Allow
Alt-Svc
Access-Control-Allow-Origin
Access-Control-Allow-Credentials
Access-Control-Expose-Headers
Access-Control-Max-Age
Access-Control-Allow-Methods
Access-Control-Allow-Headers
```

With this file in the repo, you can call the program like this:
```bash
./buidtrie dummy.txt
```

You will find the following example output:
```text
Analyzing file: testfile.txt
 -_-c-_-_-p-t-_-P-_-_-_-_-: Accept-Patch [depth=12]
 -_-c-_-_-p-t-_-R-_-_-_-_-_-: Accept-Ranges [depth=13]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-A-_-_-_-_-_-O-_-_-_-_-_-: Access-Control-Allow-Origin [depth=27]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-A-_-_-_-_-_-C-_-_-_-_-_-_-_-_-_-_-: Access-Control-Allow-Credentials [depth=32]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-A-_-_-_-_-_-M-_-_-_-_-_-_-: Access-Control-Allow-Methods [depth=28]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-A-_-_-_-_-_-H-_-_-_-_-_-_-: Access-Control-Allow-Headers [depth=28]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-E-_-_-_-_-_-_-_-_-_-_-_-_-_-: Access-Control-Expose-Headers [depth=29]
 -_-c-_-_-s-_-_-_-_-_-_-_-_-_-_-M-_-_-_-_-_-_-: Access-Control-Max-Age [depth=22]
 -_-g-_-: Age [depth=3]
 -_-l-l-_-_-: Allow [depth=5]
 -_-l-t-_-_-_-_-: Alt-Svc [depth=7]
```
The output shows the characters from the lines in between '-' characters. If
the original lines include '-' characters, they will also appear between '-'
such that you might get '---'. This only happens if that character is
necessary for uniquely distinguish it from another string of course.

In this output, you can see that some characters are skipped, and some
characters are printed. For instance, in the first line the characters
'cptP' are printed, because at those positions in the string the characters
deviate from another string that had exactly the same prefix. 

In theory terms, a trie is built of all the lines in the file. All trie nodes
are annotated with the amount of actual values (lines in the input file) can
be found on any path from the trie node (in all it's descendants). When the
amount of actual values on such a path changes from parent to child, it means 
that two strings had the same prefix, but diverged on the child node. This
child node is then imporant to distinguish between strings, and is printed to
stdout. Nodes where this value does not change when moving from parent to child
are nodes that do not contribute anything to the uniqueness, and are skipped by
printing a '_' character.

Is it optimal? I don't know, I haven't created a formal proof. Will I ever?
Maybe, when I find myself with plenty of free time and a scratch pad nearby.
