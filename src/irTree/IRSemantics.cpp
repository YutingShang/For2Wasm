#include "IRSemantics.h"
#include <string>
#include <cfloat>
#include <iostream>

/////////////////////////BASIC METHODS/////////////////////////

bool IRSemantics::isPosInteger(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool IRSemantics::isPosInt32(const std::string &s) {
    //INT_MAX is 2147483647
    if (!isPosInteger(s)) return false;    //its a float 

    if (s.length() <= 10){
        return true;
    }
    else{
        try{
            std::stoi(s);
        }
        catch (const std::out_of_range &e){
            return false;
        }
        catch (const std::exception &e){
            throw;     //rethrow any other exceptions
        }
        return true;
    }
}

bool IRSemantics::isPosInt64(const std::string &s) {
    if (!isPosInteger(s)) return false;    //its a float 
    if (s.length() <= 19){
        return true;
    }
    else{
        try{
            std::stoll(s);
        }
        catch (const std::out_of_range &e){
            return false;
        }
        catch (const std::exception &e){
            throw;     //rethrow any other exceptions
        }
        return true;
    }
}

bool IRSemantics::isPosFloat32(const std::string &s) {
    if (!isPosFloat(s)) return false;    //its not a float
    if (s.length() <= 10){
        return true;
    }
    else{
        try{
            std::stof(s);
        }
        catch (const std::out_of_range &e){
            return false;
        }
        catch (const std::exception &e){
            throw;     //rethrow any other exceptions
        }
        return true;
    }
}

bool IRSemantics::isPosFloat64(const std::string &s) {
    if (!isPosFloat(s)) return false;    //its not a float
    if (s.length() <= 19){
        return true;
    }
    else{
       try{
            std::stod(s);
        }
        catch (const std::out_of_range &e){
            return false;
        }
        catch (const std::exception &e){
            throw;     //rethrow any other exceptions
        }
        return true;
    }
}

bool IRSemantics::isPosFloat(const std::string &s) {
    if (s.empty()) return false;
    
    // Check if there's exactly one decimal point
    size_t decimalCount = std::count(s.begin(), s.end(), '.');
    if (decimalCount != 1) return false;
    
    // Check if all other characters are digits
    for (char c : s) {
        if (!std::isdigit(c) && c != '.') return false;
    }
    return true;
}

bool IRSemantics::isStringConstant(const std::string &s)
{
    return !s.empty() && s[0] == '$';
}


/////////////////////////VARIABLE METHODS/////////////////////////

bool IRSemantics::isVariable(const std::string &s) {
    
    //it is a variable if it is not any of the values
    if (s.empty() || isPosInteger(s) || isPosFloat(s) || isStringConstant(s)) {
        return false;
    }

    return true;
}

bool IRSemantics::isInternalTempVar(const std::string &s)
{
    return isVariable(s) && s.length() > 1 && s[0] == '_' && s[1] == 't';     //2 types of temp variables, _t and _s (which will show in the program)
}

bool IRSemantics::isCompilerGeneratedVar(const std::string &s)
{
    return isVariable(s) && s.length() > 1 && s[0] == '_' && s[1] == 's';     //2 types of temp variables, _t and _s (which will show in the program)
}

bool IRSemantics::isProgramVariable(const std::string &s) {
    return isVariable(s) && !isInternalTempVar(s);   //not an internal temp variable
}

bool IRSemantics::isUserDefinedVar(const std::string &s) {
    return isProgramVariable(s) && !isCompilerGeneratedVar(s);   //not a compiler-generated variable
}


/////////////////////////IR TEXT METHODS/////////////////////////


bool IRSemantics::isValidArithOp(const std::string& op) {
    return op == "ADD" || op == "SUB" || op == "MUL" || op == "DIV";
}

bool IRSemantics::isValidLogicBinOp(const std::string& op) {
    return op == "OR" || op == "AND";
}

bool IRSemantics::isValidRelOp(const std::string& op) {
    return op == "EQ" || op == "NE" || op == "LT" || op == "LE" || op == "GT" || op == "GE";
}

bool IRSemantics::isValidEndBlock(std::string text) {
    return text == "ENDTHEN" || text == "ENDELSE" || text == "ENDIF" || text == "ENDLOOP" || text == "ENDBODY";
}

bool IRSemantics::isValidDatatype(const std::string& type) {
    return type == "INT" || type == "INT64" || type == "REAL" || type == "DOUBLE";
}

/////////////////////////OTHER METHODS/////////////////////////

std::string IRSemantics::getVectorText(std::vector<std::string> vector) {
    if (vector.empty()) {
        throw std::runtime_error("Cannot get vector text for empty vector");
    }
    std::string tupleText = "(";
    for (const std::string& value : vector) {
        tupleText += value + ", ";
    }
    // Remove the last comma and space
    tupleText.pop_back();
    tupleText.pop_back();
    tupleText += ")";
    return tupleText;
}

std::string IRSemantics::findLargestDatatype(const std::string& type1, const std::string& type2) {
    std::vector<std::string> highestPriority = {"DOUBLE", "REAL", "INT64", "INT"};
    for (int i=0; i<highestPriority.size() ; i++){
        if (type1 == highestPriority[i] || type2 == highestPriority[i]){
            return highestPriority[i];    //stop and return the highest priority datatype
        }
    }
    throw std::runtime_error("Could not find largest datatype between "+type1+" and "+type2);
}

