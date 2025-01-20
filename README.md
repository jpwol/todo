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

## Building

Once you have cJSON, just change the CMakeLists.txt to path to where the header and library files are (if static linking, otherwise include the .dll in the project directory).
In the project root directory, run the command:

```bash
cmake . -G "Unix Makefiles" -B build
cd build
make
```

which will create a bin folder for you and place the binary within.
