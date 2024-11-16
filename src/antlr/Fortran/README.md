# Fortran Antlr 

Antlr is a tool for generating lexers and parsers from grammar files, which I will use to generate a lexer and parser for the Fortran 90 language for my compiler.

First, I downloaded the ANTLR tool itself - the `antlr-4.13.2-complete.jar` file from the [ANTLR Getting Started Guide](https://github.com/antlr/antlr4/blob/master/doc/getting-started.md) and added it to the project. This Java program is used to generate lexers, parsers, and related files from grammar (.g4) files.

## Interpreter Lexer and Parser
To quickly try out parsing with the grammar, you can use an interpreter with the following command (after setting up an alias for `antlr4` to use the `.jar` file):
```
antlr4-parse Fortran90Parser.g4 Fortran90Lexer.g4 program -gui easymath.f90
```

Options:
- `program`: The name of the grammar rule to start parsing with. In this case, it's the `program` rule in the grammar file.
- `-gui`: Use the graphical interface
- `-tree`: Print the tree of the parsed program
- `-tokens`: Print the tokens of the parsed program
- `-trace`: Print the trace of the parser
- `easymath.f90`: The file to parse.

## Compiled Lexer and Parser

To generate the lexer and parser from the grammar file (if any changes are made to the grammar file), run the following command:
```
antlr4 -Dlanguage=Cpp Fortran90Parser.g4 Fortran90Lexer.g4
```

Options:
- `-Dlanguage=Cpp`: Generate C++ code
- `-no-listener`: Do not generate a listener
- `-visitor`: Generate a visitor
- `-o`: Output directory


This will generate c++ files including:
- `Fortran90Lexer.h`
- `Fortran90Lexer.cpp`
- `Fortran90Parser.h`
- `Fortran90Parser.cpp`

I also downloaded the [Fortran90LexerBase](https://github.com/antlr/grammars-v4/tree/master/fortran/fortran90/Cpp) from the grammar-v4 github repo and added it to the project. This is the base class for the Lexer class.

---

The Fortran90 grammar file required transformation to work with the C++ target. Specifically:
- Added required C++ header includes like `#include "Fortran90LexerBase.h"`
- Used an existing Python script `transform.py` from the ANTLR repo to handle the transformation automatically



## Antlr Runtime Library

The runtime is needed to actually run the lexer and parser, generated from the Antlr tool.

1. **Download the Runtime**  
   Download the ANTLR runtime library from the [ANTLR website](https://www.antlr.org/download.html) for your platform. Move the folder into `usr/local/lib` such that
   - The runtime header files (`antlr4-runtime.h`) are in `/usr/local/lib/antlr4-runtime`
   - The dynamic libraries are in `/usr/local/lib/lib`

2. **Recompile for Specific Platforms (Optional)**  
   If the precompiled dynamic library is not suitable for your platform, you can recompile it. For example, on an Apple Silicon Mac with macOS 13.0:


    ```
    git clone https://github.com/antlr/antlr4.git
    cd antlr4/runtime/Cpp
    mkdir build
    cd build
    cmake .. -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
    make
    ```

    After building, copy the following libraries into your library directory (e.g., /usr/local/lib/lib):

    ```
    libantlr4-runtime.4.13.2.dylib
    libantlr4-runtime.a
    libantlr4-runtime.dylib
    ```



3. **Using and Linking the Runtime**

    To use the compiled lexer and parser in the project `main.cpp`, include the following headers:

    ```
    #include "antlr4-runtime.h"
    #include "Fortran90LexerBase.h"
    #include "Fortran90Lexer.h"
    #include "Fortran90Parser.h"
    ```

    Then compile the project with the following command:
    ```
    g++ -std=c++17 -I/usr/local/lib/antlr4-runtime -I./include -L/usr/local/lib/lib -lantlr4-runtime -o main main.cpp Fortran90Lexer.cpp Fortran90Parser.cpp Fortran90LexerBase.cpp
    ```

    The compilation flags explained:
    - `-I`: Specifies directories containing header files (for runtime, lexer, and parser)
    - `-L`: Specifies directories containing library files (for runtime)
    - `-lantlr4-runtime`: Links against the ANTLR4 runtime dynamic library
