//
//  Parser.hpp
//  slowjs
//
//  Created by zx on 2021/4/19.
//

#ifndef Parser_hpp
#define Parser_hpp

#include <stdio.h>
#include <iostream>
#include <string>
#include "Tokenizer.hpp"

using namespace std;

namespace nt
{
    enum Node_Type
    {
        Program,
        StatementList,
        Statement,
        EmptyStatement,
        VariableDeclaration,
        ExpressionStatement,
        FunctionDeclaration,
        FormalParameterList,
        ForStatement,
        IfStatement,
        BlockStatement,
        BreakStatement,
        ContinueStatement,
        ReturnStatement,
        AssignmentExpression,
        Literal,
        Identifier,
        BinaryExpression,
        UpdateExpression,
        NewExpression,
        CallExpression,
        MemberExpression,
        Arguments,
        ThisExpression,
        UnaryExpression,
        FunctionExpression,
        ThrowStatement,
        WhileStatement,
        DoWhileStatement,
        ConditionalExpression,
        ArrayExpression,
        ObjectExpression,
        ObjectKeyValue,
    };
}

namespace rt
{
    enum Raw_Type
    {
        Default,

        Number,
        String,
        Boolean,
        Null,
        Undefined,
    };
}

class AST_Node
{
public:
    AST_Node(nt::Node_Type t, string v = "AST_Node_default_value") : type(t), value(v){};

    nt::Node_Type type;
    string value;
    rt::Raw_Type rawType = rt::Default;
    vector<AST_Node *> childs;
    bool computed = false;
};

class Parser
{
public:
    AST_Node *root = nullptr;
    Token *lookahead = nullptr;
    Token *current = nullptr;
    queue<Token *> tokenQueue;

    void nextToken();
    template <typename T>
    bool eat(const T &);

    bool expect(const string &);
    bool expect(tt::Token_Type);

    bool expectNot(const string &);
    bool expectNot(tt::Token_Type);

    bool semicolon();

    AST_Node *parse(const queue<Token *> &);

    AST_Node *Program();
    vector<AST_Node *> StatementList();
    AST_Node *Statement();
    AST_Node *EmptyStatement();
    AST_Node *BreakStatement();
    AST_Node *ContinueStatement();
    AST_Node *ReturnStatement();
    AST_Node *ThrowStatement();
    AST_Node *IfStatement();
    AST_Node *IterationStatement();
    AST_Node *BlockStatement();
    AST_Node *ExpressionStatement();
    AST_Node *VariableDeclaration();
    AST_Node *FunctionDeclaration();
    AST_Node *FunctionExpression();
    AST_Node *FormalParameterList();
    AST_Node *FunctionTail(AST_Node *, AST_Node *);

    AST_Node *Expression();
    AST_Node *AssignmentExpression();
    AST_Node *ConditionalExpression();
    AST_Node *LogicalExpression();
    AST_Node *EqualityExpression();
    AST_Node *RelationalExpression();
    AST_Node *AdditiveExpression();
    AST_Node *MultiplicativeExpression();
    AST_Node *UnaryExpression();
    AST_Node *PostfixExpression();
    AST_Node *LeftHandSideExpression();
    AST_Node *CallExpression();
    AST_Node *Arguments();
    AST_Node *NewExpression();
    AST_Node *MemberExpression(AST_Node *node = nullptr);
    AST_Node *MemberExpressionRight(AST_Node *);
    AST_Node *PrimaryExpression();
    AST_Node *Literal();
    AST_Node *Identifier();
    AST_Node *ArrayLiteral();
    vector<AST_Node *> ElementList();
    AST_Node *ObjectLiteral();
    vector<AST_Node *> PropertyNameAndValueList();
    AST_Node *PropertyAssignment();
    AST_Node *PropertyName();
    vector<AST_Node *> ExpressionList();

    AST_Node *buildBinary(AST_Node *, AST_Node *, const string &);
    int checkLeftHandSideValue(AST_Node *);
    int check(bool);

    static void traversal(AST_Node *node, string &prefix);
    static void printAST(AST_Node *node);
};

#endif /* Parser_hpp */
