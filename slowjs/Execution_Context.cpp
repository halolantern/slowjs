//
//  Execution_Context.cpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#include "Execution_Context.hpp"

bool Environment_Record::HasBinding(const string &name)
{
    return this->find(name) != this->end();
}

void Environment_Record::CreateMutableBinding(const string &name)
{
    this->insert(pair<string, JSValue>(name, JS_UNDEFINED));
};

void Environment_Record::SetMutableBinding(const string &name, const JSValue &value)
{
    Environment_Record::iterator it = this->find(name);
    if (it == this->end())
        this->insert(pair<string, JSValue>(name, value));
    else
        it->second = value;
};

JSValue Environment_Record::GetBindingValue(const string &name)
{
    Environment_Record::iterator it = this->find(name);
    return it == this->end() ? JS_UNDEFINED : it->second;
}

bool Object_ER::HasBinding(const string &name)
{
    return this->binding_obj->HasProperty(name);
}

void Object_ER::CreateMutableBinding(const string &name)
{
    this->binding_obj->Put(name, JS_UNDEFINED);
};

void Object_ER::SetMutableBinding(const string &name, const JSValue &value)
{
    this->binding_obj->Put(name, value);
};

JSValue Object_ER::GetBindingValue(const string &name)
{
    return this->binding_obj->Get(name);
}
