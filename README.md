# Task Tracker CLI
This is C++ beginner project from [roadmap](https://roadmap.sh/projects/task-tracker)

A simple command-line interface (CLI) application built in C++ to track and manage your tasks. This project demonstrates filesystem interaction, JSON handling, and CLI argument parsing without external frameworks.

## Features
- **Add, Update, and Delete** tasks.
- **Mark tasks** as `todo`, `in-progress`, or `done`.
- **Filter tasks** by status.
- **Persistent storage** using a local `tasks.json` file.

## Project Structure
- `taskCLI.cpp`: Main entry point and CLI logic.
- `utils.cpp / .h`: Helper functions.
- `libs/nlohmann/json.hpp`: Header-only library for JSON manipulation.

## How to Build
This project uses **CMake**. To build it locally:

1. Create a build directory:
   ```bash
   mkdir build
   cd build
2. Generate build files
   ```bash
   cmake ..
3. Compile the project:
   ```bash
   cmake --build .
The executable taskCLI.exe (or taskCLI on Linux/Mac) will be generated in the build folder.

## Usage Example
* Adding a new task
  ```bash
   taskCLI add "Buy groceries"
Output: Task added successfully (ID: 1)

* Updating and deleting tasks
  ```bash
   taskCLI update 1 "Buy groceries and cook dinner"
   taskCLI delete 1

* Marking a task as in progress or done
  ```bash
   taskCLI mark 1 in-progress
   taskCLI mark 2 done

* Listing all tasks
  ```bash
   taskCLI list

* Listing tasks by status
  ```bash
   taskCLI list done
   taskCLI list todo
   taskCLI list in-progress
