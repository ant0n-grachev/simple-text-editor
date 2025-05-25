# 📝 Simple C Text Editor

A command-line text editor written in C with support for basic editing commands and undo functionality.

## Features

- View, append, insert, replace, and delete lines
- Undo operations using a stack of previous states
- Load/save text files to disk
- Clean and modular ANSI C codebase
- Written with dynamic memory management and linked lists
- No external libraries needed.
- Memory-safe and leak-checked.

## Getting Started

### 🛠 Build Instructions

Make sure you have `clang` or `gcc` installed.

```bash
make
```
This will compile the project and produce the editor executable.

### ✅ Run the Editor
```bash
./editor <filename>
```
If the file exists, it will be loaded. Otherwise, a new file will be created.

## Commands

| Command       | Description                     |
| ------------- | ------------------------------- |
| `help` or `?` | Show available commands         |
| `list`        | Show text with line numbers     |
| `append`      | Add a line to the end           |
| `insert`      | Add a line at a specific index  |
| `replace`     | Modify an existing line         |
| `delete`      | Remove a line                   |
| `clear`       | Clear all lines (can be undone) |
| `undo`        | Revert to previous state        |
| `save`        | Save file to disk               |
| `quit` or `q` | Exit the program                |
