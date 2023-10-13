#include <stdio.h>
#include <string.h>

#pragma warning(disable : 4996);

int main(){
    char text[] = "it was   a   dark and     stormy  night"; 

    char* word = strtok(text, " ");

    while(word != NULL){
        printf("%s\n" , word); 
        word = strtok(NULL, " "); 
    }
    
    return 0; 
}
