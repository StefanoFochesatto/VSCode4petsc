# Setting Up PETSc Development in VSCode

This document outlines the steps I followed to set up my PETSc development environment for C programming in Visual Studio Code.

## 1. Download, Configure, and Build PETSc

- **Download PETSc:**  
  Follow the instructions on the [PETSc website](https://petsc.org/release/install/) to download PETSc.

- **Configure PETSc:**  
  Run the configuration script. Note that sometimes you need to reconfigure PETSc (using `./reconfigure`) to add or remove packages after the initial configuration.  
  Example:
  ```bash
  ./configure --with-cc=gcc --with-cxx=g++ --with-fc=gfortran --download-mpich --download-fblaslapack
  ```
  You may later run `./reconfigure` if you need to adjust package settings.

- **Build PETSc:**  
  After configuration, compile PETSc:
  ```bash
  make all check
  ```

## 2. Set Environment Variables

Add the PETSc environment variables to your shell configuration (e.g., `~/.bashrc`):

```bash
export PETSC_DIR=/home/stefano/petsc
export PETSC_ARCH=arch-linux-c-debug
```

After editing, source your `.bashrc`:

```bash
source ~/.bashrc
```


## 3. Download C/C++ Extensions for VSCode
There are several VSCode extensions that make it possible to develop C/C++ in VSCode. I use the suite developed my Microsoft: [https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)


## 4. Set Up VSCode Build Task

Create a folder called `.vscode` in your project directory (if it doesn't already exist). Then create a `tasks.json` file in that folder.

For a new project, you can use **Ctrl+Shift+B** to run the default build task once this file is set up. The following example is configured to build the currently active C file.

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build PETSc Program",
      "type": "shell",
      "command": "mpicc",
      "args": [
        "${workspaceFolder}/example.c",
        "-g",
        "-O0",
        "-I${env:PETSC_DIR}/include",
        "-I${env:PETSC_DIR}/${env:PETSC_ARCH}/include",
        "-L${env:PETSC_DIR}/${env:PETSC_ARCH}/lib",
        "-Wl,-rpath,${env:PETSC_DIR}/arch-linux-c-debug/lib",
        "-lpetsc",
        "-o",
        "${workspaceFolder}/example"
      ],
      "options": {
        "env": {
          "PETSC_DIR": "/home/stefano/petsc", 
          "PETSC_ARCH": "arch-linux-c-debug",
          "LD_LIBRARY_PATH":"/home/stefano/petsc/arch-linux-c-debug/lib"
        }
      },
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": ["$gcc"]
    }
  ]
}


```

### Notes:
- **Command:**  
  The build command uses `mpicc` (the MPI C compiler wrapper), which is typical for compiling PETSc programs.
- **Include & Library Paths:**  
  The task uses the environment variables `PETSC_DIR` and `PETSC_ARCH` to set up the correct include paths (`-I`) and library paths (`-L`).
- **Environment Variables in Tasks:**  
  The `"options": { "env": { ... } }` block ensures that even if VSCode does not inherit your shell’s environment, the task still gets the correct variables.
- **Debugging**
    The `-g` flag generates debug symbols and `-O0` disables optimizations that can sometimes cause the debugger to skip or inline code in unexpected ways.

## 5. Running the Code

- **Compile:**  
  Use **Ctrl+Shift+B** in VSCode to build the project. The build task will compile the current active file `example.c` into `example`

- **Running the Executable:**  
  After compilation, open a terminal (or use the integrated terminal) and run:
  ```bash
  ./example
  ```
  If you encounter an error about shared libraries (e.g., `libpetsc.so.3.22` not found), ensure your `LD_LIBRARY_PATH` includes the PETSc library directory:
  ```bash
  export LD_LIBRARY_PATH=$PETSC_DIR/arch-linux-c-debug/lib:$LD_LIBRARY_PATH
  ./main
  ```

## 6. Using the Debugger 

Inside of `.vscode` we need to setup a `.launch.json` file to run the debugger. The following is an example of a `.launch.json` file with two configuration one
for serial debugging and another for parallel debugging (which doesn't technically work and the PETSc docs suggest using Totalview for parallel debugging anyway: [https://petsc.org/release/miscellaneous/codemanagement/](https://petsc.org/release/miscellaneous/codemanagement/)). They are setup to run on the active C file. 
```
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug PETSc Program",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/${fileBasenameNoExtension}",
      "args": [
        // Place any PETSc command-line options here, for example:
        // "-dm_view", "vtk:mesh.vtu"
      ],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}",
      "environment": [
        {
          "name": "PETSC_DIR",
          "value": "/home/stefano/petsc"
        },
        {
          "name": "PETSC_ARCH",
          "value": "arch-linux-c-debug"
        },
        {
          "name": "LD_LIBRARY_PATH",
          "value": "/home/stefano/petsc/arch-linux-c-debug/lib"
        }
      ],
      "externalConsole": false,
      "MIMode": "gdb",
      "preLaunchTask": "Build Current C File",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ]
    },
    {
        "name": "Debug PETSc Program (MPI)",
        "type": "cppdbg",
        "request": "launch",
        "program": "/usr/bin/mpiexec",
        "args": [
        "-n",
        "1", // Adjust the number of processes as needed
        "${workspaceFolder}/${fileBasenameNoExtension}"
        ],
        "stopAtEntry": false,
        "cwd": "${workspaceFolder}",
        "environment": [
        {
            "name": "PETSC_DIR",
            "value": "/home/stefano/petsc"
        },
        {
            "name": "PETSC_ARCH",
            "value": "arch-linux-c-debug"
        },
        {
            "name": "LD_LIBRARY_PATH",
            "value": "/home/stefano/petsc/arch-linux-c-debug/lib"
        }
        ],
        "externalConsole": false,
        "MIMode": "gdb",
        "preLaunchTask": "Build Current C File",
        "setupCommands": [
        {
            "description": "Enable pretty-printing for gdb",
            "text": "-enable-pretty-printing",
            "ignoreFailures": true
        }
        ]
    }
  ]
}

```

## 7. Reconfiguring
PETSc offers a reconfigure script which will allow you to modify your configuration after the fact. This script is hidden pretty deep
inside the `$PETSC_ARCH` directory,
```
$PETSC_DIR/$PETSC_ARCH/lib/petsc/conf/reconfigure-$PETSC_ARCH.py
```
The docs suggest to symlink this script into your `$PETSC_DIR` but I don't want to be navigating there everytime I need to reconfigure PETSc so 
we'll setup a command in `~/.bash.rc`. Here is how to set that up,
```
nano ~/.bashrc
```
Append this to the end. 
```
petsc_reconfigure() {
    ( cd "$PETSC_DIR" && python "$PETSC_DIR/$PETSC_ARCH/lib/petsc/conf/reconfigure-$PETSC_ARCH.py" "$@" )
}
```
Then run
```
source ~/.bashrc
```
Now from anywhere you can run the reconfigure script with the following, 
```
$ petsc_reconfigure

```
