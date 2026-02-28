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
./todo add "Buy groceries"
./todo list
```