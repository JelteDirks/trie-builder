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
Example `testfile.txt` file:
```text
Accept-Patch
Accept-Ranges
Access-Control-Max-Age
Cache-Control
ETag
```

With this file in the repo, you can call the program like this:
```bash
./buidtrie testfile.txt
```

You will find the following example output:
```text
Analyzing file: testfile.txt
|0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12
|* |A |_ |_ |_ |p |_ |_ |P |_ |_ |_ |_ : Accept-Patch [depth=12]
|0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13
|* |A |_ |_ |_ |p |_ |_ |R |_ |_ |_ |_ |_ : Accept-Ranges [depth=13]
|0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13|14|15|16|17|18|19|20|21|22
|* |A |_ |_ |_ |s |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ : Access-Control-Max-Age [depth=22]
|0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13
|* |C |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ |_ : Cache-Control [depth=13]
|0 |1 |2 |3 |4 
|* |E |_ |_ |_ : ETag [depth=4]
```
The output shows the characters from the lines in between '-' characters. If
the original lines include '-' characters, they will also appear between '-'
such that you might get '---'. This only happens if that character is
necessary for uniquely distinguishing the string from another string of course.
The depth of all the characters are printed above the characters. This makes
the result a little more complex but a lot easier to work with as a reference
for programming...

In theory terms, a trie is built of all the lines in the file. All trie nodes
are annotated with the amount of actual values (lines in the input file) can
be found on any path from the trie node (in all it's descendants). When the
amount of actual values on such a path changes from parent to child, it means 
that two strings had the same prefix, but diverged on the child node. This
child node is then imporant to distinguish between strings, and is printed to
stdout. Nodes where this value does not change when moving from parent to child
are nodes that do not contribute anything to the uniqueness, and are skipped by
printing a '_' character.

It is not optimal in case of comparisons, counter examples can be found where
fewer comparisons are needed to uniquely determine a string.
