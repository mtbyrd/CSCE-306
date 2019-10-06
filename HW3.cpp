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
		cout << "Constructing a Stmt object" << endl;
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
		cout << "Getting Stmt name" << endl;
		return name;
	}
	void setName (string s){
		cout << "Setting Stmt name" << endl;
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
//Michaela wrote the InputStmt class
class InputStmt : public Stmt{
private:
	string var;
public:
	InputStmt(string vname)
		:Stmt("t_input"){
		cout << "Constructing an InputStmt object" << endl;
		var = vname;
	}
	~InputStmt();
	string toString(){
		return "Name: " + Stmt::getName() + " Variable Name: " + var;
	}
	void execute(){
		cout << "Executing InputStmt" << endl;
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
class StrOutStmt : public Stmt{
private:
	string value;
public:
	StrOutStmt();
	~StrOutStmt();
	string toString();
	void execute();
};
//Michaela wrote the ExprOutStmt class
class ExprOutStmt : public Stmt{
private:
	Expr* p_expr;
public:
	ExprOutStmt(Expr* e)
	:Stmt("t_output"){
		cout << "Constructing ExprOutStmt" << endl;
		p_expr = e;
	}
	~ExprOutStmt(){
		cout << "Deconstructing ExprOutStmt" << endl;
		delete (p_expr);
	}
	string toString(){
		cout << "COnverting ExprOutStmt to string" << endl;
		return "Name: " + Stmt::getName() + " Expression: " + p_expr->toString();
	}
	void execute(){
		cout << "Executing ExprOutStmt" << endl;
		cout << p_expr->eval() << endl;
		pc++;
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
//Michaela wrote the WhileStmt class
class WhileStmt : public Stmt{
private:
	Expr* p_expr;
	int elsetarget;
public:
	WhileStmt(Expr* e)
	:Stmt("t_while"){
		cout << "Constructing a WhileStmt object" << endl;
		p_expr = e;
		elsetarget = -1;
	}
	~WhileStmt(){
		cout << "Destructing a WhileStmt object" << endl;
		delete(p_expr);
	}
	string toString(){
		cout << "Converting a WhileStmt object to string" << endl;
		return "Name: " + Stmt::getName() + " Condition: " + p_expr->toString() + " Target: " + to_string(elsetarget);
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
//Michaela wrote the GoToStmt class
class GoToStmt : public Stmt{
private:
	int gotoline;
public:
	GoToStmt(int x)
	:Stmt("t_end"){
		//pre: constructs a statement that instructs program to go to a certain instruction. Parameter is the index of that instruction in insttable
		//post: object is created
		cout << "Constructing a GoToStmt" << endl;
		gotoline = x;
	}
	~GoToStmt(){cout << "Deconstructing a GoToStmt" << endl;}
	string toString (){
		cout << "Converting a GoToStmt to string" << endl;
		//Eclipse will allow concatenation with an int- Tim's compiler does not like it
		return "Name: " + Stmt::getName() + " Going to: " + to_string(gotoline);
	}
	void execute (){
		cout << "Executing a GoToStmt" << endl;
		pc = gotoline;
	}
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
		cout << "Constructing IdExpr object" << endl;
		id = s;
	}
	int eval(){
		cout << "Evaluating IdExpr object" << endl;
		return vartable[id];
	}
	string toString(){
		cout << "Converting IdExpr object to string" << endl;
		return "Data Type: " + symboltable[id] + " ID: " + id + " Value: " + to_string(vartable[id]);
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
	//Michaela wrote buildWhile
	WhileStmt* buildWhile(){
		cout << "Building while" << endl;
		Expr* condition = buildExpr();
		tokitr++; lexitr++;
		//Incremented to "t_loop" so further statements can be read
		return new WhileStmt(condition);
	}
	//Stmt* buildStmt(); -- Not used
	AssignStmt* buildAssign();
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
			return new StrOutStmt ();
		}
		else{
			return new ExprOutStmt(buildExpr());
		}
	}
	//Michaela wrote buildExpr
	Expr* buildExpr(){
		cout << "Building expression" << endl;
		//Assumes that iterators are pointing directly to the start of an expression-inside any parens
		vector <Expr*> exprs;
		vector <string> ops;
		while (*tokitr!="s_rparen" && *tokitr!="s_semi"){
			//Analyzing a new lexeme
			if (*tokitr=="t_id"){
				exprs.push_back(new IdExpr(*lexitr));
			}
			else if (*tokitr=="t_number" || *tokitr=="t_true" || *tokitr=="t_false"){ //bool
				exprs.push_back(new ConstExpr(stoi(*lexitr)));
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
		return new InFixExpr(exprs, ops);;
	}
	void populateTokenLexemes(istream& infile);
	void populateSymbolTable(istream& infile);
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
			if (*tokitr=="t_while"){
				tokitr++; lexitr++;
				//Increments pointers to the start of the expression
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
				tokitr++; lexitr++;
				//Incremented to where the id is
				insttable.push_back(buildInput());
				//this is incremented to the start of the parens
			}
			else if (*tokitr=="t_output"){
				tokitr++; lexitr++;
				tokitr++; lexitr++;
				//Incremented to expression or string
				insttable.push_back(buildOutput());
			}
			//safe to assume that the final t_end is going to be tokens.end?
			else if (*tokitr=="t_end"){
				tokitr++; lexitr++;
				int line = open_stmts.top();
				open_stmts.pop();
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
		return true;
	}
	void run();  		// executes the instruction table
};



int main() {
        cout << "Arugula" << endl; // prints Arugula
	return 0;
}



