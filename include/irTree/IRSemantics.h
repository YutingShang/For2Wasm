#pragma once

#include <string>
#include <vector>

//some static methods that help consistently define some of the syntax and semantics that my IR supports
//e.g. validation, supported types, etc.

//debrief of stuff in my IR:

///VALUES: parser removes all the negative values, so we don't need to worry about them!
// string constants (e.g. $hello, $world, etc.)
// positive integer constants (e.g. 1, 2, 3, etc.)
// positive floating point constants (e.g. 1.0, 2.0, 3.0, etc.)

///VARIABLES:
// user-defined program variables
// compiler-generated program variables (e.g. _s1, _s2, _s3, etc.)
// internal temp variables (e.g. _t1, _t2, _t3, etc.)

///OPERATIONS:
// arithmetic operations: ADD, SUB, MUL, DIV
// binary logic operations: OR, AND
// relational operations: EQ, NE, LT, LE, GT, GE

///TYPES:
// INT, INT64, REAL, DOUBLE


class IRSemantics {
    public:

        //VALUES
        static bool isPosInt32(const std::string &s);
        static bool isPosInt64(const std::string &s);
        static bool isPosInteger(const std::string &s);          //1, 2, 3, etc.
        static bool isPosFloat32(const std::string &s);
        static bool isPosFloat64(const std::string &s);
        static bool isPosFloat(const std::string &s);                //1.0, 2.0, 3.0, etc.
        static bool isStringConstant(const std::string& item);       //$hello, $world, etc.

        //VARIABLES
        static bool isVariable(const std::string &s);                //any type of variable - if not a value, then it is a variable
        static bool isInternalTempVar(const std::string &s);         //_t1, _t2, _t3, etc.
        static bool isCompilerGeneratedVar(const std::string &s);    //_s1, _s2, _s3, etc.
        static bool isProgramVariable(const std::string &s);         //variable that is not an internal temp variable
        static bool isUserDefinedVar(const std::string &s);          //program variable that is not a compiler-generated (e.g. x, y, z, etc.)

        //IR INSTRUCTIONS
        static bool isValidArithOp(const std::string& op);
        static bool isValidLogicBinOp(const std::string& op);
        static bool isValidRelOp(const std::string& op);
        static bool isValidDatatype(const std::string& type);
        static bool isValidEndBlock(std::string text);

        //OTHER METHODS
        static std::string getVectorText(std::vector<std::string> vector);   //turn vector into a tuple looking string (dim1, dim2, dim3, ...) or (index1, index2, index3, ...)
       
};
