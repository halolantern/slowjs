//
//  Spec_Type.hpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#ifndef Spec_Type_hpp
#define Spec_Type_hpp

#include <stdio.h>
#include <string>
#include "JSValue_Type.hpp"
#include "Execution_Context.hpp"

typedef union BaseUnion
{
    JSValue *js_value;
    Environment_Record *env_recored;
} BaseUnion;

enum Base_Type
{
    BASE_TYPE_JSVALUE,
    BASE_TYPE_RECORD,
};

// https://262.ecma-international.org/5.1/#sec-8.7
class Reference
{
public:
    Reference(Environment_Record *record, string _name) : name(_name), _base_type(BASE_TYPE_RECORD)
    {
        base.env_recored = record;
    }
    Reference(JSValue *value, string _name) : name(_name), _base_type(BASE_TYPE_JSVALUE)
    {
        base.js_value = value;
    }
    Base_Type getBaseType() const { return _base_type; };

    BaseUnion base;
    string name;

private:
    Base_Type _base_type;
};

// https://262.ecma-international.org/5.1/#sec-8.10
class PropertyDescriptor
{
public:
    JSValue Enumerable = JS_FALSE;
    JSValue Configurable = JS_FALSE;
};

class DataDescriptor : public PropertyDescriptor
{
public:
    DataDescriptor(JSValue v) : Value(v){};
    JSValue Value = JS_UNDEFINED;
    JSValue Writable = JS_FALSE;
};
class AccessorDescriptor : public PropertyDescriptor
{
public:
    JSFunction Get;
    JSFunction Set;
};

#endif /* Spec_Type_hpp */
