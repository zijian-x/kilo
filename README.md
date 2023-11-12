# Kilo Editor

## Overview

Kilo Editor is a simple text editor written in C++ for educational purposes.
It is a pseudo-follow-along of the
[snaptoken's kilo tutorial](https://github.com/snaptoken/kilo-tutorial)
from Paige Ruten based on
[antirez's kilo editor](http://antirez.com/news/108).

Both the tutorial, and the original kilo editor is written in pure C with no
dependencies. For my own interest, this version of the `kilo` editor is
completely written in modern C++(C++20) with [minimal dependencies](#Dependencies).

## Features

- **Basic Text Editing:** essential text editing functionalities such as
  insertion, deletion, and navigation.
- **Syntax Highlighting :** basic syntax highlighting for `C/C++`

## Special Attributes

- Using my implementation of string data structure `str` instead of
  `<string>` from the STL:
    - implementation of `<string>` member functions such as:
	- `replace`
	- `erase`
	- `find`
	- `rfind`
	- `compare`
	- `insert`
	- `append`
	- `push_back`
	- `pop_back`
    - iterator support: fully supports iterator-functions from the STL
- Heavy use of modern C++ paradigms:
    - *RAII:* for completely avoiding manual management of memory allocation
    - *move-semantic:* for avoiding unnecessary copies of data
    - *template methods:* for compile-time branching based on argument type
    - *constant expressions*
    - *lambda functions*
- Adhering to the best practices of writing modern C++ such as:
    - *copy-and-swap idiom*
    - *the rule of zero* or *the rule of five*

## Dependencies

- Minimal use of The C++ Standard Library: for functions from headers such as
  `<unistd>`, `<ctype>`, `<algorithm>`, `<format>` etc.

### Example output of `ldd <kilo binary>`

```sh
linux-vdso.so.1 (0x00007fffc37fc000)
libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x00007fc808800000)
libm.so.6 => /usr/lib/libm.so.6 (0x00007fc80980b000)
libresolv.so.2 => /usr/lib/libresolv.so.2 (0x00007fc8097fa000)
libgcc_s.so.1 => /usr/lib/libgcc_s.so.1 (0x00007fc8097d5000)
libc.so.6 => /usr/lib/libc.so.6 (0x00007fc80861e000)
/lib64/ld-linux-x86-64.so.2 => /usr/lib64/ld-linux-x86-64.so.2 (0x00007fc809918000)
```

## Getting Started

### Prerequisites

- C++ compiler (C++11 or later)
- make

### Building

`make`

### Running

Run the compiled executable: `./bin/kilo`

## Usage

- **Text Editing:** Open the editor and start typing. Use arrow keys to
  navigate, and press `Ctrl-Q` to quit.

## Acknowledgments

- The Kilo Editor is based on the work of [antirez](https://github.com/antirez),
  the original creator of Kilo.
- Special thanks to the Paige Ruten for the amazing follow-along tutorial!

## Disclaimer

This project is intended for educational purposes only. Use it responsibly and
at your own risk.
