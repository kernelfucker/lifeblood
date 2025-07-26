# lifeblood
attempts passwds on luks-encrypted volumes

# compile
$ clang lifeblood.c -o lifeblood -Wall -Werror -Os -s

# usage
$ ./lifeblood -f passwds -l /dev/sdX

$ ./lifeblood -f passwds -l /dev/sdX -o output

# options
```
-f    passwd file
-l    luks volume
-o    save true passwd to file
```

# example
<img width="390" height="230" alt="image" src="https://github.com/user-attachments/assets/fab3779a-f15c-4e08-834d-634aae72d0b6" />
