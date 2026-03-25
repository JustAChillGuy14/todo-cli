# todo-cli

A simple CLI to-do manager written in C.

## Getting started

Clone the repository:

```sh
git clone https://github.com/JustAChillGuy14/todo-cli.git
cd todo-cli
```

### Building:

```sh
cc -o todo todo.c

# Or use clang

clang -o todo todo.c
```

## Usage

```sh
./todo --help       # Print usage
./todo add --help   # Print usage of add
./todo list --help  # Print usage of list
./todo done --help  # Print usage of done
./todo rm --help    # Print usage of rm
```

## Example

```sh
./todo add "Complete homework"
./todo list
./todo add "Complete English homework" --sublist 1 # Add task to a specific sublist(parent) with `-s`/`--sublist`
./todo add "Complete Science homework" -s 1
./todo list -s 1 # View all children on 1
```