
//============================================================================
// Name        : HW3.cpp
// Author      : Michaela Byrd
// Version     :
// Copyright   : Description:
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <stack>
#include <cmath>

using namespace std;


// You will need these forward references.
class Expr;
class Stmt;

// Runtime Global Variables
int pc;  				// program counter
vector<string> lexemes;
vector<string> tokens;
vector<string>::iterator lexitr;
vector<string>::iterator tokitr;
map<string, int> vartable; 	// map of variables and their values
vector<Stmt *> insttable; 		// table of instructions
map<string, string> symboltable; // map of variables to datatype (i.e. sum t_integer)


// Runtime Global Methods
void dump(); 				// prints vartable, instable, symboltable

//Function to convert our infix expression to postfix expression
string convertInfixToPostfix(string infix);
// Function to determine operator precedence
int operatorPrecedence(char ch);
//Function to evaluate the post expression
int evaluatePostfixExpression(string postfix);
// Helper function to do the calculation in
// the evaluatePostfixExpression(string postfix) method
int calculate(int a, int b, char operatorSign);

// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.

class Expr{ // expressions are evaluated!
public:
    virtual int eval(){
        return -1;
    }
    virtual string toString(){
        return "Error";
    }
    virtual ~Expr(){}
};



class Stmt{ // statements are executed!
private:
    string name;
public:
    Stmt(string n){
        //cout << "Constructing a Stmt object" << endl;
        name = n;
    }
    Stmt(Expr* e){}
    Stmt(int x){}
    virtual ~Stmt(){};

    virtual string toString() = 0;
    virtual void execute() = 0;
    string getName (){
        //pre:
        //post: name member variable is returned
        return name;
    }
    void setName (string s){
        //pre: the parameter is the name member variable
        //post: name has been set to s
        name = s;
    }
};

//Tim wrote the AssignStmt class
class AssignStmt : public Stmt {
private:
    string var;
    Expr *p_expr;
public:
    AssignStmt(string v, Expr *e);
    ~AssignStmt();
    string toString();
    void execute();
};
// pre: none
// post:
//
//
//
AssignStmt::AssignStmt(string vname, Expr *expr)
        : Stmt("s_assign") {
    //cout << "in AssignStmt copy constructor" << endl;
    var = vname;
    p_expr = expr;
}

AssignStmt::~AssignStmt() {
    cout << "in AssignStmt destructor" << endl;
    if (p_expr != nullptr)
        delete p_expr;
}

void AssignStmt::execute() {
    vartable[var] = p_expr->eval();
    pc++;
}

string AssignStmt::toString() {
    string output = "";
    output += "Name: " + getName() + " Variable: " + var +
              " Value: " + p_expr->toString();
    return output;
}

//Michaela wrote the InputStmt class
class InputStmt : public Stmt{
private:
    string var;
public:
    InputStmt(string vname)
            :Stmt("t_input"){
        //cout << "Constructing an InputStmt object" << endl;
        var = vname;
    }
    ~InputStmt(){};
    string toString(){
        return "Name: " + Stmt::getName() + " Variable Name: " + var;
    }
    void execute(){
        //cout << "Executing InputStmt" << endl;
        string val;
        //Assuming I don't need to do anything else specific for this
        cout << "Enter value for of type " << symboltable[var]<< " for: " << var << endl;
        cin >> val;
        if (symboltable[var]=="t_boolean"){
            if (val=="false" || val == "0"){
                vartable[var] = 0;
            }
            else {
                vartable[var] = 1;
            }
        }
        else{
            vartable[var] = stoi(val);
        }
        pc++;
    }
};

//Tim wrote the StrOutStmt class
class StrOutStmt : public Stmt {
private:
    string value;
public:
    StrOutStmt(string val);
    ~StrOutStmt() {};
    string toString();
    void execute();
};

StrOutStmt::StrOutStmt(string val)
        : Stmt("t_strout") {
    //cout << "In  StrOutStmt copy constructor" << endl;
    value = val;
}

string StrOutStmt::toString() {
    string output = getName() + " " + value;
    return output;
}

void StrOutStmt::execute() {
    cout << value << endl;
    pc++;
}

//Michaela wrote the ExprOutStmt class
class ExprOutStmt : public Stmt{
private:
    Expr* p_expr;
public:
    ExprOutStmt(Expr* e)
            :Stmt("t_output"){
        //cout << "Constructing ExprOutStmt" << endl;
        p_expr = e;
    }
    ~ExprOutStmt(){
        cout << "Deconstructing ExprOutStmt" << endl;
        if(p_expr != nullptr)
            delete (p_expr);
    }
    string toString(){
        //cout << "COnverting ExprOutStmt to string" << endl;
        return "Name: " + Stmt::getName() + " Expression: " + p_expr->toString();
    }
    void execute(){
        //cout << "Executing ExprOutStmt" << endl;
        cout << p_expr->eval() << endl;
        pc++;
    }
};

//Tim wrote the IfStmt  class
class IfStmt : public Stmt {
private:
    Expr *p_expr;
    int elsetarget;
public:

    IfStmt(Expr *e);

    ~IfStmt();

    string toString();

    void execute();

    void setElseTarget(int x);
};

IfStmt::IfStmt(Expr *expr)
        : Stmt("t_if") {
    //cout << "in IfStmt copy constructor" << endl;
    p_expr = expr;
    elsetarget = -1;
}

IfStmt::~IfStmt() {
    //cout << "in IfStmt destructor" << endl;
    if (p_expr != nullptr)
        delete p_expr;
}

void IfStmt::execute() {
    if(p_expr->eval())
        pc++;
    else
        pc = elsetarget;
}
string IfStmt::toString() {
    return "Name: " + getName() + " Condition: " + p_expr->toString()
           + " Tagert: " + to_string(elsetarget);
}

void IfStmt::setElseTarget(int x) {
    elsetarget = x;
}

//Michaela wrote the WhileStmt class
class WhileStmt : public Stmt{
private:
    Expr* p_expr;
    int elsetarget;
public:
    WhileStmt(Expr* e)
            :Stmt("t_while"){
        //cout << "Constructing a WhileStmt object" << endl;
        p_expr = e;
        elsetarget = -1;
    }
    ~WhileStmt(){
        //cout << "Destructing a WhileStmt object" << endl;
        delete(p_expr);
    }
    string toString(){
        //cout << "Converting a WhileStmt object to string" << endl;
        return "Name: " + Stmt::getName() + " Condition: " + p_expr->toString() + " Target: " + to_string(elsetarget);
    }
    void setTarget(int x){
        //pre: The parameter is the index of the next instruction to execute after the while loop fails its condition.
        //post: elsetarget is set to x
        //cout << "Setting elsetarget in WhileStmt" << endl;
        elsetarget = x;
    }
    void execute(){
        //cout << "Executing a WhileStmt" << endl;
        //pc is set to the start of the while loop coming in
        while (p_expr->eval()){
            //Assumes that pc is incremented in eval, else increment pc here
            while (pc<elsetarget){
                insttable[pc]->execute();
                pc++;
            }
        }
    }
};

//Michaela wrote the GoToStmt class
class GoToStmt : public Stmt{
private:
    int gotoline;
public:
    GoToStmt(int x)
            :Stmt("t_end"){
        //pre: constructs a statement that instructs program to go to a certain instruction. Parameter is the index of that instruction in insttable
        //post: object is created
        //cout << "Constructing a GoToStmt" << endl;
        gotoline = x;
    }
    ~GoToStmt(){cout << "Deconstructing a GoToStmt" << endl;}
    string toString (){
        //cout << "Converting a GoToStmt to string" << endl;
        //Eclipse will allow concatenation with an int- Tim's compiler does not like it
        return "Name: " + Stmt::getName() + " Going to: " + to_string(gotoline);
    }
    void execute (){
        //cout << "Executing a GoToStmt" << endl;
        pc = gotoline;
    }
};

// Tim wrote the
class ConstExpr : public Expr {
private:
    int value;
public:
    ConstExpr(int val);

    int eval();

    string toString();
};

ConstExpr::ConstExpr(int val) {
    //cout << "In  ConstExpr conversion constructor" << endl;
    value = val;
}

int ConstExpr::eval() {
    return value;
}

string ConstExpr::toString() {
    return to_string(value);
}

// Michaela wrote the IdExpr class
class IdExpr : public Expr{
private:
    string id;
public:
    IdExpr(string s){
        //cout << "Constructing IdExpr object" << endl;
        id = s;
    }
    int eval(){
        //cout << "Evaluating IdExpr object" << endl;
        return vartable[id];
    }
    string toString(){
        //cout << "Converting IdExpr object to string" << endl;
        return "Data Type: " + symboltable[id] + " ID: " + id + " Value: " + to_string(vartable[id]);
    }
};

class InFixExpr : public Expr {
private:
    vector<Expr *> exprs;
    vector<string> ops;  // tokens of operators
public:
    InFixExpr(vector<Expr *> exprV, vector<string> opsV) {
        exprs = exprV;
        ops = opsV;
    };

    ~InFixExpr() {
        //cout << "In InFixExpr destructor" << endl;
        exprs.clear();
    };

    int eval();

    string toString();

    bool compareAndRelate(int value, int opsIndex);

    int computeRelation(int leftOperand, int rightOperand, string op);
};

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
                if(i == 0)
                    value = exprs[i]->eval();
                else
                    value = evaluatePostfixExpression(convertInfixToPostfix(infix));
                return compareAndRelate( value, i);
            }
        }
        //cout << infix << endl;
        value = evaluatePostfixExpression(convertInfixToPostfix(infix));
    }
    return value;
}

bool InFixExpr::compareAndRelate(int l_value, int ops_index) {
    bool condition = false;
    if (ops_index == (ops.size() - 1)) {
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
            inFixExpr += " " + ops[i] + " " ;
        }
    }
    return inFixExpr;
}

class Compiler{
private:
    //Tim wrote buildIf
    IfStmt* buildIf(){
        cout << "Building if" << endl;
        Expr* condition = buildExpr();
        tokitr++; lexitr++;
        //Incremented to "t_loop" so further statements can be read
        return new IfStmt(condition);
    }
    //Michaela wrote buildWhile
    WhileStmt* buildWhile(){
        cout << "Building while" << endl;
        Expr* condition = buildExpr();
        tokitr++; lexitr++;
        //Incremented to "t_loop" so further statements can be read
        return new WhileStmt(condition);
    }
    //Stmt* buildStmt(); -- Not used
    //Tim wrote buildAssign
    AssignStmt* buildAssign(string var){
        //cout << *tokitr << "  "  << *lexitr << endl;
        cout << "Building assignstmt" << endl;
        Expr* assign = buildExpr();
        //cout << *tokitr << " " << *lexitr << endl;
        //tokitr++; lexitr++;
        //cout << *tokitr << " " << *lexitr << endl;
        //cout << assign->toString() << endl;
        //Incremented to "t_loop" so further statements can be read
        return new AssignStmt(var, assign);
    }
    //Michaela wrote buildInput
    InputStmt* buildInput(){
        cout << "Building input" << endl;
        string id = *lexitr;
        tokitr++; lexitr++;
        //incremented to closing paren so next instruction can be read
        return new InputStmt (id);
    }
    //Michaela wrote buildOutput
    Stmt* buildOutput(){
        cout << "Building output" << endl;
        if (*tokitr=="t_string"){
            return new StrOutStmt (*lexitr);
        }
        else{
            return new ExprOutStmt(buildExpr());
        }
    }
    //Michaela wrote buildExpr
    Expr* buildExpr(){
        bool constant = false;
        bool variable = false;
        cout << "Building expression" << endl;
        //Assumes that iterators are pointing directly to the start of an expression-inside any parens
        vector <Expr*> exprs;
        vector <string> ops;
        string tempValue = "";
        while (*tokitr!="s_rparen" && *tokitr!="s_semi"){
            //Analyzing a new lexeme
            if (*tokitr=="t_id"){
                exprs.push_back(new IdExpr(*lexitr));
                tempValue= *lexitr;
                variable = true;
            }
            else if (*tokitr=="t_number" || *tokitr=="t_true" || *tokitr=="t_false"){ //bool
                exprs.push_back(new ConstExpr(stoi(*lexitr)));
                constant = true;
            }
            else if (*tokitr=="s_plus" || *tokitr=="s_minus" || *tokitr=="s_mult" || *tokitr=="s_div" || *tokitr=="s_mod" || *tokitr=="t_and" || *tokitr=="t_or"){
                //If the token/lexeme indicates an aritmetic operator
                ops.push_back(*lexitr);
            }
            else {
                //once a relop is reached, an infix expression is created
                InFixExpr* temp = new InFixExpr(exprs, ops);
                exprs.clear();
                ops.clear();
                exprs.push_back(temp);
                ops.push_back(*lexitr);
            }
            tokitr++; lexitr++;
        }
        if(ops.size() == 0){
            if(constant){
                return new ConstExpr(exprs[0]->eval());
            }else{
                return new IdExpr(tempValue);
            }
        }else
            return new InFixExpr(exprs, ops);;
    }
    //Tim wrote populateTokenLexemes
    void populateTokenLexemes(istream& infile){
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
    void populateSymbolTable(istream& infile){
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
public:
    Compiler(istream& source, istream& symbols){
        populateTokenLexemes(source);
        populateSymbolTable(symbols);
    }
    bool compile(){
        cout << "Compiling program" << endl;
        // builds the instruction table and declaration check
        stack <int> open_stmts;
        tokitr = tokens.begin();
        lexitr = lexemes.begin();
        while (tokitr!=tokens.end()){
            //cout << *tokitr << " " << *lexitr << endl;
            if (*tokitr=="t_while"){
                tokitr++; lexitr++;
                tokitr++; lexitr++;
                //Increments pointers to the start of the expression
                open_stmts.push(insttable.size());
                insttable.push_back(buildWhile());
            }
            else if (*tokitr=="t_if"){
                tokitr++; lexitr++;
                tokitr++; lexitr++;
                open_stmts.push(insttable.size());
                insttable.push_back(buildIf());
            }
            else if (*tokitr=="t_input"){
                tokitr++; lexitr++;
                tokitr++; lexitr++;
                //Incremented to where the id is
                insttable.push_back(buildInput());
                //this is incremented to the start of the parens
            }
            else if (*tokitr=="t_output"){
                tokitr++; lexitr++;
                tokitr++; lexitr++;
                //Incremented to expression or string
                //cout << *lexitr << endl;
                insttable.push_back(buildOutput());
                //cout << *lexitr << endl;
            }
            else if (*tokitr=="t_id"){
                //Uses current index of lexitr so does not increment here-
                // increments in buildAssign
                string var = *lexitr;
                tokitr++; lexitr++;
                if(*tokitr=="s_assign"){
                    tokitr++; lexitr++;
                    insttable.push_back(buildAssign(var));
                }
            }
                //safe to assume that the final t_end is going to be tokens.end?
            else if (*tokitr=="t_end"){
                //cout << "Almost done" << endl;
                tokitr++; lexitr++;
                if(tokitr != tokens.end()) {
                    int line = open_stmts.top();
                    open_stmts.pop();
                    if (*tokitr == "t_loop") {
                        WhileStmt *ws = dynamic_cast<WhileStmt *>(insttable[line]);
                        if (ws != nullptr) {
                            ws->setTarget(insttable.size());
                        }
                        //Need condition for failed cast?
                        insttable.push_back(new GoToStmt(line));
                    } else {
                        // t_if
                        IfStmt *is = dynamic_cast<IfStmt *>(insttable[line]);
                        if (is != nullptr) {
                            is->setElseTarget(insttable.size());
                        }
                    }
                    tokitr++; lexitr++;
                }
            }
            else{
                tokitr++; lexitr++;
                //incremented to start of new instructions
            }
        }
        return true;
    }
    //Tim wrote run
    void run(){ 		// executes the instruction table
        cout << "\nRunning program" << endl;
        pc = 0;
        while(pc != insttable.size()) {
            //cout << pc << endl;
            insttable[pc]->execute();
        }
    }
};

void dump() {
    cout << "\nPrinting the var table" << endl;
    map<string, int>::iterator itr;
    for (itr = vartable.begin(); itr != vartable.end(); ++itr) {
        cout << '\t' << itr->first << "\t" << itr->second << '\n';
    }
    cout << "\nPrinting the instruction table" << endl;
    for (int i = 0; i < insttable.size(); i++) {
        cout << insttable[i]->toString() << endl;
    }
    cout << "\nPrinting the Symbol table" << endl;
    map<string, string>::iterator itr2;
    for (itr2 = symboltable.begin(); itr2 != symboltable.end(); ++itr2) {
        cout << '\t' << itr2->first << "\t" << itr2->second << '\n';
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


int main() {
    ifstream infile("data2.txt");
    ifstream symbolFile("s_tablein.txt");
    if (!infile || !symbolFile) {
        cout << "error opening input file - token/lexeme pairs" << endl;
        exit(-1);
    }
    Compiler comp(infile, symbolFile);
    cout << comp.compile() << endl;
    dump();
    comp.run();
    return 0;
}


