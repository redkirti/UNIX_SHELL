# UNIX_SHELL
Implementation of UNIX Shell
# Simple Shell Program

## Overview
This project is a basic shell program implemented in C. It can accept user input, tokenize it, and execute commands as a simple command-line interpreter. The shell also handles `SIGINT` signals to terminate only the foreground process.

## Features
- **Tokenization**: Splits input commands into tokens based on spaces, newlines, tabs, and the `&` character.
- **Process Management**: Forks a new process for each command and executes it using `exec()` family functions.
- **Signal Handling**: Handles `Ctrl+C` (`SIGINT`) gracefully to terminate the foreground process.

## Requirements
- **C Compiler**: GCC or any C compiler.
- **Operating System**: Linux or any Unix-like system.

## Compilation
To compile the shell program, use the following command:

```bash
gcc -o my_shell my_shell.c
