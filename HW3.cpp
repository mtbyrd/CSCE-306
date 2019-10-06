//
//  main.cpp
//  Project3
//
//  Created by Timothe Byukusenge on 9/24/19.
//  Copyright © 2019 Timothe Byukusenge. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <cmath>

using namespace std;

// You will need these forward references.

class Stmt;

class Expr;

// Runtime Global Variables
int pc;                  // program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, int> vartable;     // map of variables and their values
vector<Stmt *> insttable;         // table of instructions
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)


// Runtime Global Methods

void dump();                 // prints vartable, instable, symboltable

//Function to convert our infix expression to postfix expression
string convertInfixToPostfix(string infix);
// Function to determine operator precedence
int operatorPrecedence(char ch);
//Function to evaluate the post expression
int evaluatePostfixExpression(string postfix);
// Helper function to do the calculation in
// the evaluatePostfixExpression(string postfix) method
int calculate(int a, int b, char operatorSign);

// additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.
class Expr { // expressions are evaluated!
public:
    Expr() {};

    virtual int eval() = 0;

    virtual string toString() = 0;

    virtual ~Expr() {}
};

class Stmt { // statements are executed!
private:
    string name;
public:
    Stmt() {}

    Stmt(string n) {
        name = n;
    }

    void setName(string n) {
        name = n;
    }

    string getName() {
        return name;
    }

    virtual ~Stmt() {};

    virtual string toString() = 0;

    virtual void execute() = 0;
};


//Member 1: AssignStmt, StrOutStmt, IfStmt, ConstExpr, Run
class AssignStmt : public Stmt {
private:
    string var;
    Expr *p_expr;
public:
    AssignStmt();

    AssignStmt(string v, Expr *e);

    ~AssignStmt();

    string toString();

    void execute();
};

AssignStmt::AssignStmt()
        : Stmt("s_assign") {
    cout << "In assignstmt constructor" << endl;
};

// pre: none
// post:
//
//
//
AssignStmt::AssignStmt(string v, Expr *expr)
        : Stmt("s_assign") {
    cout << "in AssignStmt copy constructor" << endl;
    var = v;
    p_expr = expr;
}

AssignStmt::~AssignStmt() {
    cout << "in AssignStmt destructor" << endl;
    if (p_expr != nullptr)
        delete p_expr;
}

void AssignStmt::execute() {
    vartable[var] = p_expr->eval();
    //cout << vartable[var]  << " To double check" << endl;
}

string AssignStmt::toString() {
    string output = "";
    output += getName() + " " + var + " " + p_expr->toString();
    return output;
}


class InputStmt : public Stmt {
private:
    string var;
public:
    InputStmt();

    ~InputStmt();

    string toString();

    void execute();
};

//Member 1: AssignStmt, StrOutStmt, IfStmt, ConstExpr, Run
class StrOutStmt : public Stmt {
private:
    string value;
public:
    StrOutStmt() {
        cout << "In  StrOutStmt constructor" << endl;
    };

    StrOutStmt(string val);

    ~StrOutStmt() {};

    string toString();

    void execute();
};

StrOutStmt::StrOutStmt(string val)
        : Stmt("t_strout") {
    cout << "In  StrOutStmt copy constructor" << endl;
    value = val;
}

string StrOutStmt::toString() {
    string output = getName() + " " + value;
    return output;
}

void StrOutStmt::execute() {
    cout << value << endl;
}


class ExprOutStmt : public Stmt {
private:
    Expr *p_expr;
public:
    ExprOutStmt();

    ExprOutStmt(Expr *expr);

    ~ExprOutStmt();

    string toString();

    void execute();
};

ExprOutStmt::ExprOutStmt()
        : Stmt("t_exprout") {

};

ExprOutStmt::ExprOutStmt(Expr *expr)
        : Stmt("t_exprout") {
    cout << "In  StrOutStmt copy constructor" << endl;
    p_expr = expr;
}

ExprOutStmt::~ExprOutStmt(){

}

string ExprOutStmt::toString() {
    string output = getName() + " " + p_expr->toString();
    return output;
}

void ExprOutStmt::execute() {
    cout << p_expr->eval() << endl;
}


//Member 1: AssignStmt, StrOutStmt, IfStmt, ConstExpr, Run
class IfStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;
public:
    IfStmt();

    IfStmt(Expr *e);

    ~IfStmt();

    string toString();

    void execute();

    void setElseTarget(int x);
};

IfStmt::IfStmt() {
}

IfStmt::IfStmt(Expr *expr)
        : Stmt("t_if") {
    cout << "in AssignStmt copy constructor" << endl;
    p_expr = expr;
    elsetarget = -1;
}


IfStmt::~IfStmt() {
    cout << "in IfStmt destructor" << endl;
    if (p_expr != nullptr)
        delete p_expr;
}

void IfStmt::execute() {

}

string IfStmt::toString() {
    return getName() + " " + p_expr->toString() + " " + to_string(elsetarget);
}

void IfStmt::setElseTarget(int x) {
    elsetarget = x;
}

class WhileStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;
public:
    WhileStmt();

    ~WhileStmt();

    string toString();

    void execute();
};


//Member 1: AssignStmt, StrOutStmt, IfStmt, ConstExpr, Run
class ConstExpr : public Expr {
private:
    int value;
public:
    ConstExpr(int val);

    int eval();

    string toString();
};

ConstExpr::ConstExpr(int val) {
    cout << "In  ConstExpr conversion constructor" << endl;
    value = val;
}

int ConstExpr::eval() {
    return value;
}

string ConstExpr::toString() {
    return to_string(value);
}

class IdExpr : public Expr {
private:
    string id;
public:
    IdExpr() {
        cout << "In IdExpr default constructor" << endl;
    }

    IdExpr(string s);

    int eval();

    string toString();
};

IdExpr::IdExpr(string s) {
    cout << "In  IdExpr conversion constructor" << endl;
    id = s;
}

int IdExpr::eval() {
    int value = vartable[id];
    if(symboltable[id] == "t_integer"){
        //cout << "In IdExpr::eval() integer " << endl;
        return value;
    }
    else{
        //cout << "HIdExpr::eval() boolean " << endl;
        return (value != 0);
    }
}

string IdExpr::toString() {
    return id;
}


class InFixExpr : public Expr {
private:
    vector<Expr *> exprs;
    vector<string> ops;  // tokens of operators
public:
    InFixExpr() {
        cout << "In InFixExpr constructor" << endl;
    };

    ~InFixExpr() {
        cout << "In InFixExpr destructor" << endl;
        exprs.clear();
    };

    int eval();

    string toString();

    void setExpr(Expr *expr);

    void setOps(string op);

    bool compareAndRelate(int value, int opsIndex);

    int computeRelation(int leftOperand, int rightOperand, string op);
};

void InFixExpr::setExpr(Expr *expr) {
    cout << "In  InFixExpr setExpr()" << endl;
    exprs.push_back(expr);
    cout << "In  InFixExpr setExpr()" << endl;
}

void InFixExpr::setOps(string op) {
    cout << "Done! In  InFixExpr setOps()" << endl;
    ops.push_back(op);
}

int InFixExpr::eval() {
    int value;
    bool condition = false;
    if (ops.size() == 0)
        value = exprs[0]->eval();
    else{
        string infix = to_string(exprs[0]->eval());
        for (int i = 0; i < ops.size(); i++) {
            if (ops[i] == "+") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "-") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "*") {
                infix +=  ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "/") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "%") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            }else{
                //condition = true;
                if(i == 0)
                    value = exprs[i]->eval();
                else
                    value = evaluatePostfixExpression(convertInfixToPostfix(infix));
                //cout << value << " in eval()" << endl;
                return compareAndRelate( value, i);
            }
        }
        //cout << infix << endl;
        value = evaluatePostfixExpression(convertInfixToPostfix(infix));
    }
    return value;
}

bool InFixExpr::compareAndRelate(int l_value, int ops_index) {
    //cout << ops[ops_index] << " Is our index" << endl;
    bool condition = false;
    if (ops_index == (ops.size() - 1)) {
        cout << "Not sure about this shit! " << endl;
        return computeRelation(l_value, exprs[ops_index + 1]->eval(), ops[ops_index]);
    }
    else{
        string infix = to_string(exprs[ops_index+1]->eval());
        for (int i = (ops_index + 1); i < ops.size() && !condition; i++) {
            if (ops[i] == "+") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "-") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "*") {
                infix +=  ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "/") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            } else if (ops[i] == "%") {
                infix += ops[i] + to_string(exprs[i + 1]->eval());
            }else{
                condition = true;
                ops_index = i;
            }
        }
        if(!condition){
            cout << infix << endl;
            int value = evaluatePostfixExpression(convertInfixToPostfix(infix));
            return computeRelation(l_value, value, ops[ops_index]);
        } else{
            int value = evaluatePostfixExpression(convertInfixToPostfix(infix));
            return compareAndRelate( value, ops_index);
        }
    }
}
int InFixExpr::computeRelation(int leftOperand, int rightOperand, string op){
    if(op == "<")
        return leftOperand < rightOperand;
    else if(op == "<=")
        return leftOperand <= rightOperand;
    else if(op == ">")
        return leftOperand > rightOperand;
    else if(op == ">=")
        return leftOperand >= rightOperand;
    else if(op == "==")
        return leftOperand == rightOperand;
    else if(op == "!=")
        return leftOperand != rightOperand;
    else if(op == "and")
        return leftOperand && rightOperand;
    else
        return leftOperand || rightOperand;
}
string InFixExpr::toString() {
    string inFixExpr = "";
    for (int i = 0; i < exprs.size(); i++) {
        inFixExpr += exprs[i]->toString();
        if (i < ops.size()) {
            inFixExpr += ops[i];
        }
    }
    return inFixExpr;
}

class Compiler {
private:
    void buildIf();

    void buildWhile();

    void buildStmt();

    void buildAssign(string var);

    void buildInput();

    void buildOutput();

    // use one of the following buildExpr methods
    void buildExpr(Expr *&);

    Expr *buildExpr();

    void populateTokenLexemes(istream &infile);

    void populateSymbolTable(istream &infile);

public:
    Compiler(istream &source, istream &symbols) {
        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }

    bool compile();      // builds the instruction table and declaration check
    void run();          // executes the instruction table
};

bool Compiler::compile() {
    //Set the token iterators to the starting positions
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
    while (tokitr != tokens.end()) {
        if (*tokitr == "t_if") {
            *tokitr++;
            *lexitr++;
            //buildIf();
        } else if (*tokitr == "t_while") {
            *tokitr++;
            *lexitr++;
            //buildWhile();
        } else if (*tokitr == "t_id") {
            string var = *lexitr;
            *tokitr++;
            *lexitr++;
            cout << *tokitr << "\t" << *lexitr << endl;
            if (*tokitr == "s_assign")
                buildAssign(var);

            //else
            //*tokitr++; *lexitr++;
        } else if (*tokitr == "t_input") {
            *tokitr++;
            *lexitr++;
            cout << *tokitr << "\t" << *lexitr << endl;
            //buildInput();
        } else if (*tokitr == "t_output") {
            *tokitr++;
            *lexitr++;
            buildOutput();
        } else {
            *tokitr++;
            lexitr++;
        }
    }
    cout << "Done!" << endl;
    return false;
}

void Compiler::buildAssign(string var) {
    InFixExpr *inFixExpr = new InFixExpr();
    cout << "in buildAssign \t" << *lexitr << endl;
    *tokitr++;
    lexitr++;
    while (*tokitr != "s_semi") {
        if (*tokitr == "t_number") {
            // A constant assigment
            Expr *c_expr = new ConstExpr(stoi(*lexitr));
            inFixExpr->setExpr(c_expr);
        } else if (*tokitr == "t_id") {
            // A variable assigment
            Expr *id_Expr = new IdExpr(*lexitr);
            inFixExpr->setExpr(id_Expr);
        } else {
            // Operators can be
            //  s_plus, s_minus, s_mult, s_div, or s_mod
            //  s_lt, s_le, s_gt, s_ge, s_eq, s_ne
            //  t_and, t_or
            inFixExpr->setOps(*lexitr);
        }
        *tokitr++;
        lexitr++;
        cout << *tokitr << "\t" << *lexitr << endl;
    }
    cout << "Setting the instruction table" << endl;
    Stmt *assignStmt = new AssignStmt(var, inFixExpr);
    insttable.push_back(assignStmt);
    // Update the iterator
    tokitr++;
    lexitr++;
}

void Compiler::buildOutput() {
    //ExprOutStmt* expr_Stmt = new ExprOutStmt();
    // Update the iterator
    tokitr++;
    lexitr++;
    cout << "in buildOutput \t" << *lexitr << endl;
    cout << *tokitr << "\t" << *lexitr << endl;
    if (*tokitr == "t_id") {
        //We have an id output
        Expr *id_Expr = new IdExpr(*lexitr);
        Stmt *exprOut = new ExprOutStmt(id_Expr);
        insttable.push_back(exprOut );
        //expr_Stmt->
    } else {
        cout << *tokitr << "\t" << *lexitr << endl;
        //We have a string  output
        Stmt *strOut = new StrOutStmt(*lexitr);
        insttable.push_back(strOut);
    }
}

void Compiler::run() {
    for (int i = 0; i < insttable.size(); i++) {
        insttable[i]->execute();
    }
}

void Compiler::populateTokenLexemes(istream &infile) {
    string line, tok, lex;
    int pos = 0;
    //getline(infile, line);
    while (getline(infile, line)) {
        pos = line.find(":");
        tok = line.substr(0, pos - 1);
        lex = line.substr(pos + 2, line.length());
        //cout  << "\t " << tok << "\t:\t" << lex << endl;
        tokens.push_back(tok);
        lexemes.push_back(lex);
    }
    //cout << pos << " " << tok << " " << lex << endl;
    tokitr = tokens.begin();
    lexitr = lexemes.begin();
}

void Compiler::populateSymbolTable(istream &infile) {
    string line, tok, lex;
    int pos = 0;
    //getline(infile, line);
    while (getline(infile, line)) {
        pos = line.find(":");
        tok = line.substr(0, pos - 1);
        lex = line.substr(pos + 2, line.length());
        symboltable[lex] = tok;
    }
}

//Member 1: AssignStmt, StrOutStmt, IfStmt, ConstExpr, Run
int main() {
    ifstream infile("data2.txt");
    ifstream symbolFile("s_tablein.txt");
    if (!infile || !symbolFile) {
        cout << "error opening input file - token/lexeme pairs" << endl;
    }
    Compiler comp(infile, symbolFile);
    map<string, string>::iterator itr;
    for (itr = symboltable.begin(); itr != symboltable.end(); ++itr) {
        cout << '\t' << itr->first << "\t" << itr->second << '\n';
    }
    cout << comp.compile() << endl;
    cout << insttable.size() << endl;
    dump();
    comp.run();
    return 0;
}

void dump() {
    for (int i = 0; i < insttable.size(); i++) {
        cout << insttable[i]->toString() << endl;
    }
}



/*
    Infix notation to Postfix notation conversion.
    Postfix notation evaluation to get result of that expression.
    Using stack data structure in C++ language.
    Programmed by Hasan Abdullah
    Modified by Tim Byukusenge and Michaela Byrd
    Contact: http://hellohasan.com
*/


//INFIX to POSTFIX conversion
string convertInfixToPostfix(string infix)
{
    string postfix = "";
    stack <char> myStack;
    string term = "";
    char ch;
    if(infix[0] == '-'){
        infix = "0"+infix;
    }
    for(int i=0; i < infix.length() ; i++)
    {
        ch = infix[i];
        if(ch=='(') //if found opening bracket
            myStack.push(ch);
        else if(ch==')') //if found closing bracket
        {
            while(!myStack.empty() && myStack.top()!='(')
            {
                postfix = postfix + myStack.top();
                myStack.pop();
            }

            if(!myStack.empty() && myStack.top()=='(')
                myStack.pop();

        }
        else //found operator or operand
        {
            int priority = operatorPrecedence(ch); //^ is highest, then * and /. then + and -

            if(priority==0) //found operand
                postfix = postfix + ch ;
            else //found operator
            {
                postfix = postfix + " " ;
                if(myStack.empty())
                    myStack.push(ch);
                else
                {
                    while(!myStack.empty()
                          && myStack.top()!='('
                          && priority<=operatorPrecedence(myStack.top()))
                    {
                        postfix = postfix + myStack.top() + " ";
                        myStack.pop();
                    }
                    //postfix = postfix + " " ;
                    myStack.push(ch);
                }
            }
        }
    }
    while(!myStack.empty())
    {
        postfix = postfix + " " + myStack.top();
        myStack.pop();
    }
    return postfix;
}


// POSTFIX evaluation to get result
int evaluatePostfixExpression(string postfix)
{
    stack <int> myStack;
    string term;
    char ch;
    //1 2 3 ^ 4 * + 3 /
    for(int i=0; i < postfix.length() ; i++)
    {
        ch = postfix[i];
        if((ch>='0' && ch<='9') || ch == ' ' ) {
            if(ch == ' ' && term != ""){
                myStack.push(stoi(term));
                term = "";
            }else{
                term.push_back(ch );
            }
        }
        else //found operator
        {
            int a,b;
            b = myStack.top();
            myStack.pop();

            a = myStack.top();
            myStack.pop();

            myStack.push(calculate(a, b, ch));
        }
    }
    return myStack.top();
}


// Calculation, according to arithmetic operator sign
int calculate(int a, int b, char operatorSign)
{
    if(operatorSign=='+')
        return a+b;
    else if(operatorSign=='-')
        return a-b;
    else if(operatorSign=='*')
        return a*b;
    else if(operatorSign=='/')
        return a/b;
    else if(operatorSign=='^')
        return pow(a,b);

}


// Operator precedence. You can add more operator like % (MOD)
int operatorPrecedence(char ch)
{
    if(ch=='^')
        return 3; //highest priority for exponential operator
    else if(ch=='/' || ch=='*')
        return 2; //high priority than + or - operator
    else if(ch=='+' || ch=='-')
        return 1; //lowest priority among operators
    else
        return 0; //works for operands

}


