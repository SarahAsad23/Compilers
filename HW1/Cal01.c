#include <stdio.h>
#include <ctype.h>

int main() {
    while (1) {
        printf("Enter expression: ");
        
        char input[100];
        char* current = input;
        char op = '+';
        double result = 0;

        fgets(input, sizeof(input), stdin);

        while (*current != '\0') {
            while (*current == ' ') {
                current++;
            }

            double num = 0;

            if (isdigit(*current) || (*current == '-' && isdigit(*(current + 1)))) {
                if (sscanf(current, "%lf", &num) == 1) {
                    switch (op) {
                        case '+':
                            result += num;
                            break;
                        case '-':
                            result -= num;
                            break;
                        case '/':
                            result /= num;
                            break;
                        case '*':
                            result *= num;
                            break;
                        default:
                            break;
                    }

                    // Move the current pointer past the processed number
                    while (isdigit(*current) || *current == '.' || *current == '-') {
                        current++;
                    }
                }
            } else if (*current == '+' || *current == '-' || *current == '*' || *current == '/') {
                op = *current;
                current++;
            } else {
                // Move past unexpected characters
                current++;
            }
        }

        // Print the result 
        printf("Result: %lf\n", result);
    }

    return 0;
}

