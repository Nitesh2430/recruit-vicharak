/* 

    1. Variable Declarations:
       Syntax: datatype <variable_name>;
       Example: int a;

    2. Assignments:
       Syntax: <variable_name> = <expression>;
       Example: a = 10;

    3. Arithmetic Operations:
       Syntax: <operand1> <operator> <operand2>;
       Operators: + (add), - (subtract), * (multiply), / (divide)
       Example: c = a + b;

    4. Conditionals:
       Syntax:
           if (<condition>) {
               <statements>;
           } else if (<condition>) {
               <statements>;
           } else {
               <statements>;
           }
       Operators for conditions:
           == (equality), != (inequality)
           < (less than), > (greater than)
           <= (less than or equal to), >= (greater than or equal to)
       Example:
           if (c == 30) {
               d = d + 1;
           }

    5. Input/Output:
       printf is used to display results.
       Example: printf("Value: %d", a);
*/


#include <stdio.h>

int main() {
    // Variable Declarations
    int a; // Declare variable 'a' of type int
    int b; // Declare variable 'b' of type int
    int c; // Declare variable 'c' of type int
    int d; // Declare variable 'd' of type int

    // Assignments
    a = 10; // Assign value 10 to variable 'a'
    b = 20; // Assign value 20 to variable 'b'
    c = a + b; // Compute the sum of 'a' and 'b', assign to 'c'
    d = c - b; // Compute the difference of 'c' and 'b', assign to 'd'

    // Output Initial Values
    printf("Initial values:\n");
    printf("a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);

    // Conditional Statements
    if (c == 30) { // Check if 'c' equals 30
        d = d + 1; // Increment 'd' by 1 if condition is true
    } else if (c < 30) { // Check if 'c' is less than 30
        d = d - 1; // Decrement 'd' by 1 if condition is true
    } else { // Handle other cases
        d = d * 2; // Double 'd' if no previous condition is true
    }

    // Output Final Values
    printf("\nFinal values after conditionals:\n");
    printf("a = %d, b = %d, c = %d, d = %d\n", a, b, c, d);

    return 0;
}

