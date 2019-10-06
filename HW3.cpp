//============================================================================
// Name        : HW3.cpp
// Author      : Michaela Byrd
// Version     :
// Copyright   : Description:
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <stack>

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

// You may need a few additional global methods to manipulate the global variables


// Classes Stmt and Expr
// It is assumed some methods in statement and expression objects will change and
// you may need to add a few new ones.

class Stmt{ // statements are executed!
private:
	string name;
public:
	Stmt(){}
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

class AssignStmt : public Stmt{
private:
	string var;
	Expr* p_expr;
public:
	AssignStmt();
	~AssignStmt();
	string toString();
	void execute();
};

class InputStmt : public Stmt{
private:
	string var;
public:
	InputStmt(string vname){
		cout << "Constructing an InputStmt object" << endl;
		Stmt::setName("t_input");
		var = vname;
	}
	~InputStmt();
	string toString(){
		return "Name: " + Stmt::getName() + " Variable Name: " + var;
	}
	void execute(){
		string val;
		//Assuming I don't need to do anything else specific for this
		cout << "Enter value for of type " << symboltable[var]<< "for: " << var << endl;
		cin >> val;
		vartable[var] = val;
	}
};
class StrOutStmt : public Stmt{
private:
	string value;
public:
	StrOutStmt();
	~StrOutStmt();
	string toString();
	void execute();
};

class ExprOutStmt : public Stmt{
private:
	Expr* p_expr;
public:
	ExprOutStmt(Expr* e){
		cout << "Constructing ExprOutStmt" << endl;
		p_expr = e;
	}
	~ExprOutStmt(){
		cout << "Deconstructing ExprOutStmt" << endl;
		delete (p_expr);
	}
	string toString(){
		cout << "COnverting ExprOutStmt to string" << endl;
		return "Name: " + Stmt::getName() + " Expression: " + p_expr;
	}
	void execute(){
		cout << "Executing ExprOutStmt" << endl;
		cout << p_expr->eval() << endl;
	}
};

class IfStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt();
	~IfStmt();
	void setElseTarget (int);
	string toString();
	void execute();
};

class WhileStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt(Expr* e){
		cout << "Constructing a WhileStmt object" << endl;
		Stmt::setName("t_while");
		p_expr = e;
		elsetarget = -1;
	}
	~WhileStmt(){
		cout << "Destructing a WhileStmt object" << endl;
		delete(p_expr);
	}
	string toString(){
		cout << "Converting a WhileStmt object to string" << endl;
		return "Name: " + Stmt::getName() + " Condition: " + p_expr + " Target: " + elsetarget;
	}
	void setTarget(int x){
	//pre: The parameter is the index of the next instruction to execute after the while loop fails its condition.
	//post: elsetarget is set to x
		cout << "Setting elsetarget in WhileStmt" << endl;
		elsetarget = x;
	}
	void execute(){
		cout << "Executing a WhileStmt" << endl;
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

class GoToStmt : public Stmt{
private:
	int gotoline;
public:
	GoToStmt(int x){
		//pre: constructs a statement that instructs program to go to a certain instruction. Parameter is the index of that instruction in insttable
		//post: object is created
		cout << "Constructing a GoToStmt" << endl;
		Stmt::setName("t_end");
		gotoline = x;
	}
	~GoToStmt(){cout << "Deconstructing a GoToStmt" << endl;}
	string toString (){
		cout << "Converting a GoToStmt to string" << endl;
		return "Name: " + Stmt::getName() + " Going to: " + gotoline;
	}
	void execute (){
		cout << "Executing a GoToStmt" << endl;
		pc = gotoline;
	}
};


class Expr{ // expressions are evaluated!
public:
	virtual int eval() = 0;
	virtual string toString() = 0;
	virtual ~Expr(){}
};






class ConstExpr : public Expr{
private:
	int value;
public:
	ConstExpr(int val);
	int eval();
	string toString();
};

class IdExpr : public Expr{
private:
	string id;
public:
	IdExpr(string s){
		id = s;
	}
	int eval(){
		return vartable[id];
	}
	string toString(){
		return "Data Type: " + symboltable[id] + " ID: " + id + " Value: " + vartable[id];
	}
};

class InFixExpr : public Expr{
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators
public:
	InFixExpr(vector<Expr *> exprs, vector<string> ops);
	~InFixExpr();
	int eval();
	string toString();
};

class Compiler{
private:
	IfStmt* buildIf();
	//handle
	WhileStmt* buildWhile();
	//handle
	Stmt* buildStmt();
	AssignStmt* buildAssign();
	//handle
	InputStmt* buildInput();
	//handle
	Stmt* buildOutput();

	Expr* buildExpr(){
		//Assumes that iterators are pointing directly to the start of an expression-inside any parens
		vector <Expr*> exprs;
		vector <string> ops;
		while (*tokitr!="s_rparen" && *tokitr!="s_semi"){
			//Analyzing a new lexeme
			if (*tokitr=="t_id"){
				exprs.push_back(new IdExpr(*lexitr));
			}
			else if (*tokitr=="t_number" || *tokitr=="t_true" || *tokitr=="t_false"){ //bool
				exprs.push_back(new ConstExpr(*lexitr));
			}
			else{
				//If the token/lexeme indicates an operator
				ops.push_back(*lexitr);
				InFixExpr* temp = new InFixExpr(exprs, ops);
				exprs.clear();
				ops.clear();
				exprs.push_back(temp);
			}
			tokitr++; lexitr++;
		}

	}
	void populateTokenLexemes(istream& infile);
	void populateSymbolTable(istream& infile);
public:
	Compiler(istream& source, istream& symbols){
		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}
	bool compile(){
		// builds the instruction table and declaration check
		stack <int> open_stmts;
		while (*tokitr!=tokens.end()){
			if (*tokitr=="t_while"){
				tokitr++; lexitr++;
				open_stmts.push(insttable.size());
				insttable.push_back(buildWhile());
			}
			else if (*tokitr=="t_if"){
				tokitr++; lexitr++;
				open_stmts.push(insttable.size());
				insttable.push_back(buildIf());
			}
			else if (*tokitr=="t_id"){
				//Uses current index of lexitr so does not increment here- increments in buildAssign
				insttable.push_back(buildAssign());
			}
			else if (*tokitr=="t_input"){
				tokitr++; lexitr++;
				insttable.push_back(buildInput());
				//this is incremented to the start of the parens
			}
			else if (*tokitr=="t_output"){
				tokitr++; lexitr++;
				insttable.push_back(buildOutput());
			}
			//safe to assume that the final t_end is going to be tokens.end?
			else if (*tokitr=="t_end"){
				tokitr++; lexitr++;
				int line = open_stmts.pop();
				if (*tokitr=="t_loop"){
					WhileStmt* ws = dynamic_cast<WhileStmt*>(insttable[line]);
					if (ws!=nullptr){
						ws->setTarget(insttable.size());
					}
					//Need condition for failed cast?
					insttable.push_back(new GoToStmt(line));
				}
				else{
					// t_if
					IfStmt* is = dynamic_cast<IfStmt*>(insttable[line]);
					if (is!=nullptr){
						is->setElseTarget(insttable.size());
					}
				}
				tokitr++; lexitr++;
				//incremented to start of new instructions
			}
		}
	}
	void run();  		// executes the instruction table
};



int main() {
        cout << "Arugula" << endl; // prints Arugula
	return 0;
}



