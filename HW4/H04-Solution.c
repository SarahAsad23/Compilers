/*
* Sarah Asad
* CSS 448: Compilers 
* 
* Hokum is a tiny language defined by the grammar below: 
* 
* 1. Prog => Stm ; Prog | Stm    // Program
* 2. Stm => Asg | If             // Statement
* 3. Asg => Var = Exp            // Assignment
* 4. If => if Exp then Asg       // If
* 5. VorC => Var | Const         // Variable or Constant
* 6. Exp => VorC | VorC + VorC   // Expression
* 7. Var => [a-z]                // Variable
* 8. Const => [0-9]              // Constant
*/

#include <stdio.h>

typedef enum { TOKVAR = 1, TOKEQ, TOKCONST, TOKSEMI, TOKADD, TOKIF, TOKTHEN } TokKind;


// TEST GOOD 
TokKind toks[] =
{ TOKVAR, TOKEQ, TOKCONST, TOKSEMI,             // a = 3;
TOKVAR, TOKEQ, TOKVAR, TOKADD, TOKVAR, TOKSEMI, // b = a + a;
TOKIF, TOKVAR, TOKTHEN,                         // if a then
TOKVAR, TOKEQ, TOKVAR, TOKADD, TOKCONST         // x = a + 5
};

/*
// TEST BAD
TokKind toks[] = 
{ TOKVAR, TOKEQ, TOKCONST, TOKSEMI,                             // a = 3; 
TOKVAR, TOKEQ, TOKVAR, TOKADD, TOKTHEN, TOKSEMI,                // b = a + then; 
TOKIF, TOKVAR, TOKTHEN, TOKVAR, TOKEQ, TOKVAR, TOKADD, TOKCONST // if a then x = a + 5
};
*/


int tokNum = 0;     // current Token number within toks
int hiTokNum = 17;  // highest Token number within toks

char* tokToStr(TokKind k){
    if(k == TOKVAR){
        return "TOKVAR"; 
    }
    else if(k == TOKEQ){
        return "TOKEQ"; 
    }
    else if(k == TOKCONST){
        return "TOKCONST"; 
    }
    else if(k == TOKSEMI){
        return "TOKSEMI"; 
    }
    else if(k == TOKADD){
        return "TOKADD"; 
    }
    else if(k == TOKIF){
        return "TOKIF"; 
    }
    else if(k == TOKTHEN){
        return "TOKTHEN"; 
    }
    return "Invalid";
}

// pauses the program 
void pause(){
    getchar(); // used to stop the program 
}

// checks for expected token 
void mustbe(TokKind k) {
    TokKind t = toks[tokNum];
    if (t == k) {
    ++tokNum;
    } else {
    printf("Error: want %d, but found %d \n", k, t);
    pause();
    }
}

// VorC must either be a Variable or a constant 
void rexVorC(){
    TokKind t = toks[tokNum]; 
    // Var | Const
    if(t == TOKVAR){ 
        mustbe(TOKVAR); 
    }
    else if (t == TOKCONST)
    {
        mustbe(TOKCONST); 
    } 
}

// an expression must be a VorC or VorC + VorC
void rexExp(){
    // VorC | VorC + VorC
    rexVorC(); 

    if(toks[tokNum] == TOKADD){
        mustbe(TOKADD); 
        rexVorC();
    }
}

// Aissignment must follow var = exp 
void rexAsg(){
    TokKind t = toks[tokNum]; 
    // must be a variable 
    mustbe(TOKVAR); 
    // followed by an equal sign
    mustbe(TOKEQ); 
    // followed by an expression
    rexExp();  
}

// if expression then assignment 
void rexIf(){
    // must start with an if 
    mustbe(TOKIF); 
    // next should be an expression
    rexExp();
    // then 
    mustbe(TOKTHEN); 
    // assignment 
    rexAsg();
}

// should be an assignment or if 
void rexStm(){
    TokKind t = toks[tokNum]; 
    // then it is an assignment 
    if(t == TOKVAR){
        rexAsg(); 
    }
    else if(t == TOKIF){
        rexIf(); 
    }
    else {
        printf("Error: Expected TOKVAR or TOKCONST but got %s ", tokToStr(t));
        pause();
    } 
}

// can be a stm; prog or just a statment 
void rexProg(){
    // Stm ; Prog | Stm
    rexStm(); 
    // make sure we have not hit end of tokens 
    if(tokNum < hiTokNum){
        if(toks[tokNum] == TOKSEMI){
            mustbe(TOKSEMI); 
            rexProg(); 
        }
        else{
            rexStm(); 
        }
    }
}

int main() {
    // need to start at prog
    rexProg();
    printf("success\n");  
}
