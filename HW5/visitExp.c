/*
* Sarah Asad 
* CSS 448: Compilers
* HW5
*/

#include <stdio.h>

typedef struct Ast_ Ast;

typedef enum {
    ASTASG = 1, ASTARG, ASTBLOCK, ASTBODY, ASTCALL, ASTEXP, ASTFUN, ASTIF,
    ASTNAM, ASTNUM, ASTPAR, ASTPROG, ASTRET, ASTSTR, ASTTYP, ASTVAR,
    ASTWHILE
} AST;

typedef enum { // Binary Operators
    BOPADD = 1, BOPSUB, BOPMUL, BOPDIV, BOPNONE, // Arithmetic
    BOPLT, BOPLE, BOPNE, BOPEEQ, BOPGE, BOPGT // Relational
} BOP;

typedef struct Ast_ {
    AST kind;
} Ast;

typedef struct AstNum_ {
    AST kind; // ASTNUM
    int num;
} AstNum;

typedef struct AstNam_ {
    AST kind; // ASTNAM
    char* nam;
} AstNam;

typedef struct {
    AST kind; // ASTEXP
    Ast* lhs; 
    BOP bop; 
    Ast* rhs; 
} AstExp; 

// visits the num node 
void visitNum(Ast* ast){
    printf("%d ", ((AstNum*)ast)->num); 
}

// visits the nam node
void visitNam(Ast* ast){
    printf("%s ", ((AstNam*)ast)->nam); 
}

// converts each operator to its corresponding char 
char convertBop(BOP bop){
    switch (bop){
        case BOPADD:  return '+'; 
        case BOPDIV:  return '/';
        case BOPEEQ:  return '=';  
        case BOPGT:   return '>'; 
        case BOPLT:   return '<'; 
        case BOPMUL:  return '*'; 
        case BOPSUB:  return '-';
        case BOPNONE: return ' '; 
        default:      return ' '; 
    }
}

// visits the lhs, bop, and rhs and prints them out 
void visitExp (AstExp* ast){
   
    // print Exp 
    printf("Exp "); 
    
    // check if the lhs is a num or nam 
    // and call corresponding function 
    // also ensure that it is not null
    if(ast->lhs != NULL){
        if(ast->lhs->kind == ASTNUM){
        visitNum(ast->lhs); 
        }
        else if(ast->lhs->kind == ASTNAM){
        visitNam(ast->lhs);
        }
    }

    // ensure that a BOP esixts and is non 'none' 
    if(ast->bop != BOPNONE){
        // print the operator 
        printf("%c ", convertBop(ast->bop));
    } 
    
    // check if rhs is a num or nam 
    // and call the corresponding function
    // also ensure that it is not null 
    if(ast->rhs != NULL){
        if(ast->rhs->kind == ASTNUM){
        visitNum(ast->rhs); 
        }
        else if(ast->rhs->kind == ASTNAM){
        visitNam(ast->rhs); 
        }
    }

    printf("\n"); 
     
}


int main(){

    // Test 1: count * 23
    AstNam nam1 = {ASTNAM, "count"}; 
    AstNum num1 = {ASTNUM, 23}; 
    AstExp exp1 = {ASTEXP, (Ast*)&nam1, BOPMUL, (Ast*)&num1}; 
    visitExp(&exp1); 


    // Test 2: 42  
    AstNum num2 = {ASTNUM, 23}; 
    AstExp exp2 = {ASTEXP, (Ast*)&num2, BOPNONE, NULL}; 
    visitExp(&exp2);


    // Test 3: x - y
    AstNam nam3 = {ASTNAM, "x"}; 
    AstNam nam4 = {ASTNAM, "y"}; 
    AstExp exp3 = {ASTEXP, (Ast*)&nam3, BOPSUB, (Ast*)&nam4}; 
    visitExp(&exp3);


    // Test 4: 10 / 2
    AstNum num5 = {ASTNUM, 10}; 
    AstNum num6 = {ASTNUM, 2};
    AstExp exp4 = {ASTEXP, (Ast*)&num5, BOPDIV, (Ast*)&num6}; 
    visitExp(&exp4);

    // Test 5: y > 17 
    AstNam nam5 = {ASTNAM, "y"}; 
    AstNum num7 = {ASTNUM, 17}; 
    AstExp exp5 = {ASTEXP, (Ast*)&nam5, BOPGT, (Ast*)&num7}; 
    visitExp(&exp5);
}