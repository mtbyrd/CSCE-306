//============================================================================
// Name        : HW3.cpp
// Author      : Tim and Michaela
// Version     :
// Copyright   : Your copyright notice
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
map<string, int> vartable; 	// map of variables and their values- x=5
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
	InputStmt(string vname);
	~InputStmt();
	string toString();
	void execute();
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
	ExprOutStmt();
	~ExprOutStmt();
	string toString();
	void execute();
};

class IfStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	IfStmt();
	~IfStmt();
	string toString();
	void execute();
};

class WhileStmt : public Stmt{
private:
	Expr* p_expr;
	//The line to go to after the condition fails
	int elsetarget;
public:
	WhileStmt(vector<int> exprs, vector<string> opers, int go2);
	WhileStmt();
	~WhileStmt();
	string toString();
	void execute();
	void setTarget(int x);
	/*Pre: The parameter is the instruction number of the next instruction to execute after the while condition is not met
	 *Post: elsetarget is set to the instruction number of the next instruction to execute after the while condition is not met
	 */
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
	IdExpr(string s);
	int eval();
	string toString();
};

class InFixExpr : public Expr{
private:
	vector<Expr *> exprs;
	vector<string> ops;  // tokens of operators
public:
	InFixExpr();
	//populates infix expression in a default way
	InFixExpr(vector<int> nums, vector<string> opers);
	//populate infix expression with values
	~InFixExpr();
	//might want overload for boolean expressions
	//bool eval();
	//int eval();
	string toString();
};

class Compiler{
private:
	IfStmt buildIf();
	WhileStmt* buildWhile();
	void buildStmt();
	void buildAssign();
	void buildInput();
	void buildOutput();
	// use one of the following buildExpr methods
	void buildExpr(Expr*&);      Expr* buildExpr();
	void populateTokenLexemes(istream& infile);
	void populateSymbolTable(istream& infile);
public:
	Compiler(istream& source, istream& symbols){
		populateTokenLexemes(source);
		populateSymbolTable(symbols);
	}
	bool compile();  	// builds the instruction table and declaration check
	void run();  		// executes the instruction table

};

//Michaela wrote the following methods for the WhileStmt Class
WhileStmt::WhileStmt(){
	cout << "Default constructing a WhileStmt object" << endl;
	name = "t_while";
	//What do the parameters look like coming in?
	p_expr = new InFixExpr();
	elsetarget = -1;
	//when compiling, the program will use a stack to determine which end loop belongs to this and change elsetarget to the next instruction
}

void WhileStmt::WhileStmt(vector<int> exprs, vector<string> opers, int go2){
	//object instntiated with its instruction number as the elsetarget variable so it is easy for go_to to access the instruction number it needs
	cout << "3-arg constructing a WhileStmt object" << endl;
	name = "t_while";
	//What do the parameters look like coming in?
	p_expr = new InFixExpr(exprs, opers);
	elsetarget = go2;
	//when compiling, the program will use a stack to determine which end loop belongs to this and change elsetarget to the next instruction
}

//check against class code
WhileStmt::~WhileStmt(){
	cout << "Destructing WhileStmt object" << endl;
	delete(p_expr);
}

string WhileStmt::toString(){
	cout << "Converting WhileStmt to string" << endl;
	return name + " " + p_expr->toString() + " " + elsetarget;
}

void WhileStmt::setTarget(int x){
	cout << "Setting elsetarget in WhileStmt" << endl;
	elsetarget = x;
}

void WhileStmt::execute(){
	//assumes that pc is incremented in eval
	cout << "Executing WhileStmt" << endl;
	while (p_expr->eval()){
		while (pc<elsetarget){
			insttable[pc]->execute();
			pc ++;
		}
	}
	pc = elsetarget;
}

Compiler::Compiler(istream& source, istream& symbols){
	//populate vectors
}

//Michaela wrote the method compile
bool Compiler::compile(){
	stack <WhileStmt*> open_loops;
	while (*tokitr!=tokens.end()){
		if (*tokitr=="t_while"){
			tokitr++; lexitr++;
			//what to do to push a pointer?
			insttable.push_back(buildWhile());
			//built without exit line - elsetarget is the index of the whilestmt
			//nvalid arguments 'Candidates are:void push(const WhileStmt &)void push(WhileStmt &&)
			//Push pointer to while object onto stack
			open_loops.push(insttable[insttable.size()-1]);
		}
		if (*tokitr=="t_end"){
			tokitr++; lexitr++;
			if (*tokitr=="t_loop"){
				insttable.push_back(new GoTo(open_loops.pop()));
			}
		}
		//read in statements, add to insttable
		//when end loop is found, add to insttable, inc ctr, pop while loop off of stack, and set elsetarget to ctr
		else if (*tokitr=="t_if"){
			insttable.push_back(buildIf());
		}
		else if (*tokitr=="t")
		tokitr++; lexitr++;
	}
}

WhileStmt* Compiler::buildWhile(){
	//iterate token and lexeme map to get past the parentheses
	tokitr++; lexitr++;
	vector<int> exprs;
	vector<string> ops;
	if (*tokitr=="t_id"){
		exprs.push_back(vartable[*tokitr]);
	}
	else{
		exprs.push_back(stoi(*lexitr));
	}
	tokitr++; lexitr++;
	ops.push_back(*tokitr);
	tokitr++; lexitr++;
	if (*tokitr=="t_id"){
		exprs.push_back(vartable[*tokitr]);
	}
	else{
		exprs.push_back(stoi(*lexitr));
	}
	//increment past the already-read lexeme
	//increment past the rparen
	//increment past the loop
	//iterators should be at the start of the next line
	tokitr = tokitr + 3; lexitr = lexitr + 3;
	//Symbol WhileStmt could not be resolved- why?
	return new WhileStmt (exprs, ops, insttable.size());
}







//Michaela wrote the following methods for InputStmt
//Planning to write compile to pass the appropriate information to InputStmt
InputStmt::InputStmt(string vname){
	name = "t_input";
	var = vname;
}
//Delete nothing?
InputStmt::~InputStmt(){}

string InputStmt::toString(){
	return name + " " + var + " " + vartable[var];
}

void InputStmt::execute(){
	//Determine data type?
	//Do I need to call assignment stmt?
	string val;
	cout << "Enter a value for " << var << endl;
	cin >> val;
	vartable [var] = val;
}

int main() {
	cout << "Arugula" << endl; // prints Arugula
	return 0;
}






IdExpr::IdExpr(string s){


}
