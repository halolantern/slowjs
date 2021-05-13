//
//  Execution_Context.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Execution_Context_hpp
#define Execution_Context_hpp

#include <iostream>
#include <map>

#include "JSValue_Type.hpp"

class Environment_Record;
class Lexical_Environment;

class Execution_Context
{
public:
    Execution_Context(Lexical_Environment *env, JSValue thisValue)
        : lex_env(env), var_env(env), this_binding(thisValue){};

    Lexical_Environment *lex_env;
    Lexical_Environment *var_env;
    JSValue this_binding;
};

class Lexical_Environment
{
public:
    Lexical_Environment(Environment_Record *r, Lexical_Environment *o)
        : record(r), outer(o){};

    Environment_Record *record;
    Lexical_Environment *outer;
};

enum Record_Type
{
    Record_Type_Declarative,
    Record_Type_Object,
};
class Environment_Record : public map<string, JSValue>
{
public:
    Environment_Record(){};
    virtual bool HasBinding(string name);
    virtual void CreateMutableBinding(string name);
    virtual void SetMutableBinding(string name, JSValue value);
    virtual JSValue GetBindingValue(string name);

    Record_Type _tag;
};

class Object_ER : public Environment_Record
{
public:
    Object_ER(JSValue obj) : binding_obj(obj) { _tag = Record_Type_Object; };

    // bool HasBinding(string name);
    // void CreateMutableBinding(string name);
    // void SetMutableBinding(string name, JSValue value);
    // JSValue GetBindingValue(string name);
    JSValue binding_obj;
};

class Declarative_ER : public Environment_Record
{
public:
    Declarative_ER() { _tag = Record_Type_Declarative; };
};

#endif /* Execution_Context_hpp */
