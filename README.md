# TODO

todo is a weekend project I made for creating/editing/deleting todo lists in the command line. Todo creates a todo.json file, and manages task groups as well as tasks within the groups.

It's a very simple project that only parses JSON strings for the purpose of printing to the terminal.

## Dependencies

This project uses the [cJSON](https://github.com/DaveGamble/cJSON) library for JSON parsing.

### Installing cJSON

#### Windows

```bash
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON
mkdir build
cd build
cmake ..
make
```

This will give you the library files for cJSON. If not using static linking, just include the .dll in the bin with the binary to run.

The project is currently set to static link, so the CMakeLists.txt would have to be modified for cJSON if you want the static libraries as well. Simply
set DBUILD_SHARED_LIBS=ON to OFF. I also ran into issues with the c90 std having trouble with double to float conversion within cJSON.
To rememdy this, I changed the build standard to c99, and added -Wno-error=float-conversion as a build flag.

#### Linux

Either clone the [cJSON](https://github.com/DaveGamble/cJSON) repository and build it, or retreive it from your distrobution's package manager.
cJSON has documentation on building for Linux on the repository page. For retrieving from a package manager:

<details>
    <summary>Arch Based</summary>

```bash
sudo pacman -S cjson
```

</details>

<details>
    <summary>Debian Based</summary>

```bash
sudo apt install libcjson-dev
```

</details>

<details>
    <summary>Void</summary>

```bash
sudo xbps-install cJSON-devel
```

</details>

Installing these packages should install the non dev/devel as a dependency, but if not, make sure to grab those too.

## Building

If on Windows, you'll have to edit the CMakeLists.txt to point to where the cJSON header and library file are. Unix users shouldn't have to do anything if they installed cJSON through their package manager.

Once cJSON is built/installed, run this

```bash
git clone https://github.com/jpwol/todo.git
cd todo
cmake . -G "Unix Makefiles" -B build
cd build
make
```

which will create a bin folder for you and place the binary within. Adding it to your PATH will allow you to create local todo.json files for projects/directories.

## Usage

To begin, consider running "todo -h" for a list of options.

- todo -h Prints out the help screen
- todo -m \<name>\ Makes a new todo.json file in your current directory with a named todo task.
- todo -d By itself, deletes your todo.json file
  - todo -d \<task\> Deletes a task, if the task is out of bounds it will return an error.
- todo -a \<task\> Adds a string to the todo list with an index being the array's length plus 1.

### Notes

> [!TODO]
> Implement some sort of "checkoff" ability, so you can see what tasks are done instead of deleting them.
