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
JSFunction *JSObject::Object = new JSFunction("Object", (void *)Builtin_Object);

JSObject *JSObject::FunctionPrototype = new JSObject();
JSFunction *JSObject::Function = new JSFunction("Function");

void JSObject::CreateBuiltinObject()
{
    ObjectPrototype->Prototype = nullptr;
    ObjectPrototype->Put("constructor", Object->ToJSValue());

    Object->Prototype = ObjectPrototype;
    Object->Put("prototype", ObjectPrototype->ToJSValue());
    JSFunction *getPrototypeOf_fo = new JSFunction("getPrototypeOf", (void *)Builtin_GetPrototypeOf);
    Object->Put("getPrototypeOf", getPrototypeOf_fo->ToJSValue());

    Function->Put("prototype", FunctionPrototype->ToJSValue());
    FunctionPrototype->Put("constructor", Function->ToJSValue());
    JSFunction *call_fo = new JSFunction("call", (void *)Builtin_Function_Prototype_Call);
    FunctionPrototype->Put("call", call_fo->ToJSValue());
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
JSValue JSObject::DefaultValue() { return JS_UNDEFINED; };
void JSObject::DefineOwnProperty(string P, DataDescriptor *Desc)
{
    DataDescriptor *prop = GetOwnProperty(P);

    if (prop)
    {
        // TODO: HACK replace Object.prototype
        if (((JSFunction *)this)->Name == "Object" && P == "prototype" && prop->Value.getObject() == ObjectPrototype)
        {
            *ObjectPrototype = *(Desc->Value.getObject());
            ObjectPrototype->Prototype = nullptr;
        }
        else
            this->Properties.find(P)->second = Desc;
    }
    else
        this->Properties.insert(pair<string, DataDescriptor *>(P, Desc));
}

void JSFunction::initializeFunction()
{
    _tag = JS_TAG_FUNCTION;
    Prototype = JSObject::FunctionPrototype;
    JSFunction *fo = this;
    JSObject *proto = new JSObject();
    proto->DefineOwnProperty("constructor", new DataDescriptor(fo->ToJSValue()));
    fo->DefineOwnProperty("prototype", new DataDescriptor(proto->ToJSValue()));
};
JSValue JSFunction::Call(Slowjs *slow, JSValue thisValue, vector<JSValue> args)
{
    JSFunction *fo = this;
    if (fo->isIntrinsic())
        return fo->getCFunction()(Function_Data(slow, fo, thisValue, args));
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
    JSFunction *fo = this;
    JSObject *obj = new JSObject();
    JSValue proto = fo->Get("prototype");
    obj->Prototype = proto.isObject() ? proto.getObject() : JSObject::ObjectPrototype;
    JSValue result = fo->Call(slow, obj->ToJSValue(), args);
    slow->checkException(result);
    return result.isObject() ? result : obj->ToJSValue();
}
