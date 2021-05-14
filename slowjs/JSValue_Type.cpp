//
//  JSValue_Type.cpp
//  slowjs
//
//  Created by zx on 2021/4/26.
//

#include "JSValue_Type.hpp"
#include "Slowjs.hpp"
#include <iostream>

JSObject *JSObject::ObjectPrototype = new JSObject();
JSFunction *JSObject::Object = new JSFunction("Object");

JSObject *JSObject::FunctionPrototype = new JSObject();
JSFunction *JSObject::Function = new JSFunction("Function");

void JSObject::CreateBuiltinObject()
{
    ObjectPrototype->Prototype = nullptr;
    Object->Prototype = ObjectPrototype;
    Object->Put("prototype", JSValue(JS_TAG_OBJECT, ObjectPrototype));
    Object->Put("getPrototypeOf", JSValue(JS_TAG_FUNCTION, new JSFunction("getPrototypeOf")));
    ObjectPrototype->Put("constructor", JSValue(JS_TAG_FUNCTION, Object));

    FunctionPrototype->Prototype = ObjectPrototype;
    Function->Prototype = FunctionPrototype;
    Function->Put("prototype", JSValue(JS_TAG_OBJECT, FunctionPrototype));
    FunctionPrototype->Put("constructor", JSValue(JS_TAG_FUNCTION, Function));
};

DataDescriptor *JSObject::GetOwnProperty(string P)
{
    map<string, DataDescriptor *>::iterator it = this->Properties.find(P);
    if (it == this->Properties.end())
        return nullptr;
    else
        return it->second;
}
DataDescriptor *JSObject::GetProperty(string P)
{
    DataDescriptor *prop = this->GetOwnProperty(P);
    if (prop)
        return prop;
    else
    {
        JSObject *proto = this->Prototype;
        return proto ? proto->GetProperty(P) : nullptr;
    }
}
JSValue JSObject::Get(string P)
{
    DataDescriptor *desc = GetProperty(P);
    return desc ? desc->Value : JS_UNDEFINED;
}
bool CanPut(string P);
void JSObject::Put(string P, JSValue V)
{
    DataDescriptor *valueDesc = new DataDescriptor(V);
    DefineOwnProperty(P, valueDesc);
}
bool JSObject::HasProperty(string P)
{
    DataDescriptor *desc = GetProperty(P);
    return !!desc;
}
void JSObject::Delete(){};
void JSObject::DefaultValue(){};
void JSObject::DefineOwnProperty(string P, DataDescriptor *Desc)
{
    DataDescriptor *prop = GetOwnProperty(P);

    if (prop)
        this->Properties.find(P)->second = Desc;
    else
        this->Properties.insert(pair<string, DataDescriptor *>(P, Desc));
}

void JSFunction::initializeFunction()
{
    _tag = JS_TAG_FUNCTION;
    Prototype = JSObject::FunctionPrototype;
    JSFunction *fo = this;
    JSObject *proto = new JSObject();
    proto->DefineOwnProperty("constructor", new DataDescriptor(JSValue(JS_TAG_FUNCTION, fo)));
    fo->DefineOwnProperty("prototype", new DataDescriptor(JSValue(JS_TAG_OBJECT, proto)));
};
JSValue JSFunction::Call(Slowjs *slow, JSValue thisValue, vector<JSValue> args)
{
    JSFunction *fo = this;
    if (fo->isIntrinsic())
        return slow->evaluateIntrinsicFunction(fo, args);
    else
    {
        slow->initFunctionExecutionContext(fo, thisValue, args);
        AST_Node *func_body = fo->Code->childs[2];
        JSValue normal_Result = JS_UNDEFINED;
        try
        {
            normal_Result = slow->evaluate(func_body);
        }
        catch (JSValue &value)
        {
            slow->checkException(value);
            slow->ctx_stack->pop();
            return value;
        }
        slow->checkException(normal_Result);
        slow->ctx_stack->pop();
        return normal_Result;
    }
}
JSValue JSFunction::Construct(Slowjs *slow, vector<JSValue> args)
{
    JSObject *obj = new JSObject();
    JSValue proto = this->Get("prototype");
    obj->Prototype = proto.isObject() ? proto.getObject() : JSObject::ObjectPrototype;
    JSValue result = this->Call(slow, JSValue(JS_TAG_OBJECT, obj), args);
    slow->checkException(result);
    return result.isObject() ? result : JSValue(JS_TAG_OBJECT, obj);
}
