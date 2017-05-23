/***********************************************************************************
****************  Author : Mohammed Ahmed Awad Elkariem *********************************
****************        Faculty of Mathematical Sciences  ************************
****************                   July 2001             **********************
****************             SML language Project        **********************
******************************* STACK HEADER FILE NO.3 **************************************
***********************************************************************************/


#ifndef STACK_H
#define STACK_H
#include <iostream.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

template <class STACKTYPE>
class Stack //:private List<NODETYPE>
{
public:
  void push(const STACKTYPE& value) { s.insertAtFront(value);}
  bool pop(STACKTYPE& value) { return s.removeFromFront(value);}
  bool isStackEmpty() { return s.isEmpty(); }
  void printStack() {s.print();}
  STACKTYPE getStackTop() { return s.getHead()->getData();}

  friend char* convertToPostfix(Stack<char>& compilerStack,const char* infix);
  friend bool isOperator(char c);
  friend int precedence(char operator1,char operator2);
  friend int evaluatePostfixExpression(Stack<int>&,const char* postfix);
  friend int calculate(Stack<int>&,char binaryOperator);
private:
  List<STACKTYPE> s;
};

//-------------------------------------------------------------------------

 char* convertToPostfix(Stack<char>& compilerStack,const char* infix)
 {

    char* postfix = new char[2*strlen(infix)];
    int infixCounter = 0;
    int postfixCounter = 0;
    char temp;
    //stack is empty
    compilerStack.push('(');

    while(!compilerStack.isStackEmpty())
    {
       if(isdigit(infix[infixCounter]))
       {
         // get the whole number
         do
         {
            postfix[postfixCounter++] = infix[infixCounter];
            infixCounter++;
         }while(isdigit(infix[infixCounter]));
         postfix[postfixCounter++] = ' ';
         continue;  // to avoid incrementing infixCounter twice
       }
       else if(isalpha(infix[infixCounter]))
       {
         postfix[postfixCounter++] = infix[infixCounter];
         postfix[postfixCounter++] = ' ';
       }
       else if(infix[infixCounter] == '(')
         compilerStack.push('(');
       else if(isOperator(infix[infixCounter]))
       {
         temp = compilerStack.getStackTop();
         if(isOperator(temp))
         {
           int preResult = precedence(temp,infix[infixCounter]);
           while(preResult == 1 || preResult == 0)
           {
             compilerStack.pop(postfix[postfixCounter++]);
             postfix[postfixCounter++] = ' ';
             temp = compilerStack.getStackTop();
             if(isOperator(temp))
                preResult = precedence(temp,infix[infixCounter]);
             else
                break;
           }
         }
         compilerStack.push(infix[infixCounter]);
       }
       else if(infix[infixCounter] == ')')
       {
         while(compilerStack.getStackTop() != '(')
         {
           compilerStack.pop(postfix[postfixCounter++]);
           postfix[postfixCounter++] = ' ';
         }
         compilerStack.pop(temp);
       }
       infixCounter++;
   }
   postfix[postfixCounter] = '\0';
   return postfix;
 }



 //------------------------------------------------------------
 bool isOperator(char c)
 {
   if(c == '+' || c == '-' || c == '*' || c ==  '/'
      || c == '%' || c == '^')
      return true;
   else
     return false;
 }
//----------------------------------------------------------------------
 int precedence(char operator1,char operator2)
 {
   if(operator1 =='*' || operator1 == '/' || operator1 == '%')
   {
     if(operator2 == '*' ||
        operator2 == '/' ||
        operator2 == '%')
        return 0;
     else if(operator2 == '+' || operator2 == '-')
        return 1;
   }
   else if(operator1 == '+' || operator1 == '-')
   {
      if(operator2 == '+' || operator2 == '-')
         return 0;
      else if(operator2 == '*' || operator2 == '/'
         || operator2 == '%')
         return -1;
   }
   return 0;
 }
//-----------------------------------------------------------------

int evaluatePostfixExpression(Stack<int>& compilerEvaluationStack,const char* postfix)
{
   int counter = 0;
   char digitArray[10];
   int digitArrayCounter = 0;

   while(postfix[counter] != '\0')
   {
     if(isdigit(postfix[counter]))
     {
       //reset digitCounter
       digitArrayCounter = 0;
       do
       {
         digitArrayCounter++;
         counter++;
       }while(isdigit(postfix[counter]));

       // now fill digitArray with digits
       for(int  i = 0,j = counter-digitArrayCounter; i < digitArrayCounter;i++,j++)
            digitArray[i] = postfix[j];
       digitArray[digitArrayCounter] = '\0';
       int assembledInteger = atoi(digitArray);
       compilerEvaluationStack.push(assembledInteger);
     }
     else if(isOperator(postfix[counter]))
     {
         compilerEvaluationStack.push(calculate(compilerEvaluationStack,
                                           postfix[counter]));

     }

     counter++;
   }

   return compilerEvaluationStack.getStackTop();

}
//-----------------------------------------------------------------
int calculate(Stack<int>& compilerEvaluationStack,char binaryOperator)
{
   int rightOperand,leftOperand,result;
   compilerEvaluationStack.pop(rightOperand);
   compilerEvaluationStack.pop(leftOperand);

   switch(binaryOperator)
   {
     case '+':
       result = leftOperand + rightOperand;
     break;

     case '-':
       result = leftOperand - rightOperand;
     break;

     case '/':
       result = leftOperand / rightOperand;
     break;

     case '*':
       result = leftOperand * rightOperand;
     break;
   }
   return result;
}



#endif