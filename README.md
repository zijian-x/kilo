# Kilo Editor

## Overview

A simple text editor written in C++ for educational purposes.
It is a pseudo-follow-along of the
[snaptoken's kilo tutorial](https://github.com/snaptoken/kilo-tutorial)
from Paige Ruten based on
[antirez's kilo editor](http://antirez.com/news/108).

Both the tutorial, and the original kilo editor is written in pure C with no
dependencies. For my own interest, this version of the `kilo` editor is
completely written in modern C++(C++20) with [minimal dependencies](#Dependencies).

## Features

- Basic Text Editing
- Syntax Highlighting
- Incremental Searching
- Using my own implementation of string container `str` instead of `<string>`
  from the STL:
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
    - iterator support for STL algorithms
- Heavy use of modern C++ paradigms:
    - *RAII*
    - *move-semantic*
    - *template programming*
    - *lambda functions*
    - *copy-and-swap idiom*
    - *compile-time constant expressions*

## Dependencies

- Minimal use of The C++ Standard Library: for functions from headers such as
  `<algorithm>`, `<format>` etc.

## Prerequisites

- C++ compiler (C++11 or later)
- make

## Building

`make`

## Running

Run the compiled executable: `./bin/kilo`

## Usage

- Text Editing: Open the editor and start typing. Use arrow keys to navigate,
  and press `Ctrl-Q` to quit.
- Searching: Press `Ctrl-F` to initiate a search. Enter the desired word, and
  use the arrow keys (UP and DOWN) to cycle through search results.
