/*******************************************************************************
************************* Author: Mohammed Ahmed Awad Elkariem **************************
**************************** Faculty of Mathematical Sciences ***************************
********************************** MAIN PROJECT FILE ********************************
**************************** COMPILER TO THE SML LANGUAGE ***********************************
*********************************************************************************************/
#ifndef COMPILER_H
#define COMPILER_H
#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "stack.h"

#define COMMANDSNUMBER 7

//--------------------------------------------------------------------------------
class TableEntry
{
public:
  int symbol;
  char type;
  int location;

  int searchForSymbol(int _symbol,char _type)
  {
     if(symbol == _symbol && type == _type)
          return 1;
     return 0;
  }

  void insertSymbol(int _symbol,char _type,int _location)
  {
     symbol = _symbol;
     type = _type;
     location = _location;
  }

}; //End of TableEntry class

//-------------------------------------------------------------------------------
class Compiler
{
 public:
   Compiler();
   void compileStatement(char *statement);
   int isLineNumber(const char *token);
   int isSimpleCommand(const char *token);
   int getCompleteInstruction() { return operationCode * 100 + operand;}
   void evaluatePostfixExpression(char *postfix);
   void insertLetSymbols(char *postfix);
   int isInSymbolTable(int symbol,char type);
   int getSymbolLocation(int symbol,char type);
   friend isIntegerString(const char *str);
 private:
   int instructionCounter;
   int dataCounter;
   int operationCode;
   int operand;
   int smlArray[100];
   TableEntry symbolTable[100];
   int symbolTableCounter;
   const char* simpleCommand[COMMANDSNUMBER];
   Stack<char> compilerStack;

};  //End of compiler class

//--------------------------------------------------------------------------------
Compiler::Compiler()
{
  simpleCommand[0] = "rem";
  simpleCommand[1] = "input";
  simpleCommand[2] = "let";
  simpleCommand[3] = "print";
  simpleCommand[4] = "goto";
  simpleCommand[5] = "if";
  simpleCommand[6] = "goto";

  symbolTableCounter = 0;
  dataCounter = 99;
}
//----------------------------------------------------------------------------

void Compiler::compileStatement(char* statement)
{
   char *tokenPtr;
   int foundSymbol = 0;
   tokenPtr = strtok(statement," ");

   // assert that the tokenPtr is a line number later
   // then put it in the symbol table

   if(!isLineNumber(tokenPtr))
   {
     cout<<"\n ERROR. Statement not beginning with a line number";
     exit(1);
   }
   // put the line number in symbol table and don't increment instr. counter
   symbolTable[symbolTableCounter++].insertSymbol(atoi(tokenPtr),'l',instructionCounter);

   while(tokenPtr != NULL)
   {
     tokenPtr = strtok(NULL," ");
     if(isSimpleCommand(tokenPtr))
     {
       if(!strcmp(tokenPtr,"rem"))
           return; // skip compilation
       else if(!strcmp(tokenPtr,"input"))
       {
           operationCode = 10;
           tokenPtr = strtok(NULL," ");

           if(strlen(tokenPtr) == 1 && isalpha(tokenPtr[0]))
           {
              //search in symbol table
              for(int i = 0; i < symbolTableCounter ;i++)
              {
                 if(symbolTable[i].searchForSymbol(toascii(tokenPtr[0]),'v'))
                 {
                       operand = symbolTable[i].location;
                       foundSymbol = 1;
                 }
              }
              if(!foundSymbol) //insert it in symbol table if not found
              {
                      symbolTable[symbolTableCounter++].insertSymbol(toascii(tokenPtr[0]),'v',dataCounter);
                      operand  = dataCounter--;
              }

              smlArray[instructionCounter++] = getCompleteInstruction();

           }
           else
           {
              cout<<"\nERROR. After \"input\" command there must be a variable(single character)";
              exit(1);
           }
       }
       else if(!strcmp(tokenPtr,"print"))
       {
           operationCode = 11;
           tokenPtr = strtok(NULL," ");

           if(strlen(tokenPtr) == 1 && isalpha(tokenPtr[0]))
           {
              //search in symbol table
              for(int i = 0; i < symbolTableCounter ;i++)
              {
                 if(symbolTable[i].searchForSymbol(toascii(tokenPtr[0]),'v'))
                 {
                       operand = symbolTable[i].location;
                       foundSymbol = 1;
                 }
              }
              if(!foundSymbol) //insert it in symbol table if not found
              {
                      symbolTable[symbolTableCounter++].insertSymbol(toascii(tokenPtr[0]),'v',dataCounter);
                      operand  = dataCounter--;
              }

              smlArray[instructionCounter++] = getCompleteInstruction();

           }
           else
           {
              cout<<"\nERROR. After \"print\" command there must be a variable(single character)";
              exit(1);
           }
       }
       else if(!strcmp(tokenPtr,"end"))
       {
           operationCode = 43;
           operand = 0;
           smlArray[instructionCounter++] = getCompleteInstruction();
       }
       continue;
     }

     //if token is symbol generate special code
     //symbols = +,-,*,/,=,characters,digits


   }

}

//-----------------------------------------------------------------------------
int Compiler::isLineNumber(const char *token)
{
   for(int i = 0; i < strlen(token) ; i++)
   {
      if(isdigit(token[i]))
         continue;
      else
         return 0;
   }
   return 1;
}
//------------------------------------------------------------------------------

int Compiler::isSimpleCommand(const char *token)
{
  for(int i = 0; i < COMMANDSNUMBER ; i++)
     if(strcmp(token,simpleCommand[i]) == 0)
         return 1;
  return 0;
}
//--------------------------------------------------------------------------------
void  Compiler::insertLetSymbols(char *postfix)
{
   char *tokenPtr = strtok(postfix," ");
   // insert new symbols in symbolTable
   do
   {
      if(isIntegerString(tokenPtr))
         if(!isInSymbolTable(atoi(tokenPtr),'c'))
            smlArray[dataCounter--] = atoi(tokenPtr);
      else if(strlen(tokenPtr) == 1)
      {
         if(isalpha(tokenPtr[0]))       // variable
            symbolTable[symbolTableCounter++].insertSymbol(toascii(tokenPtr[0]),'v',dataCounter);
         else
           ;// it is operator
      }
     tokenPtr = strtok(NULL," ");
   }while(tokenPtr != NULL);
}
//--------------------------------------------------------------------------------

void Compiler::evaluatePostfixExpression(char *postfix)
{
   char *tokenPtr = strtok(postfix," ");

   while(tokenPtr != NULL)
   {
      if(isIntegerString(tokenPtr))
         compilerStack.push(getSymbolLocation(atoi(tokenPtr),'c'));
      else if(strlen(tokenPtr) == 1)
      {
         if(isalpha(tokenPtr[0]))
             compilerStack.push(getSymbolLocation(toascii(tokenPtr[0]),'v'));
         else if(isOperator(tokenPtr[0]))
         {
           int right,left;
           compilerStack.pop(right);
           compilerStack.pop(left);

           switch(tokenPtr[0])
           {
             case '+':
                operationCode = 20; //LOAD
                operand = left;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 30;//ADD
                operand = right;
                smlArray[instructionCounter++] = getCompleteInstruction();
                //  store result in a new location
                operationCode = 21;//STORE
                operand = dataCounter--;
                smlArray[instructionCounter++] = getCompleteInstruction();
                compilerStack.push(operand);
                break;
             case '-':
                operationCode = 20;
                operand = left;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 31;//SUBTRACT
                operand = right;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 21;
                operand = dataCounter--;
                smlArray[instructionCounter++] = getCompleteInstruction();
                compilerStack.push(operand);
                break;
             case '/':
                operationCode = 20;
                operand = left;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 32;//DIVIDE
                operand = right;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 21;
                operand = dataCounter--;
                smlArray[instructionCounter++] = getCompleteInstruction();
                compilerStack.push(operand);
                break;
             case '*':
                operationCode = 20;
                operand = left;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 33;//MULTIPLY
                operand = right;
                smlArray[instructionCounter++] = getCompleteInstruction();
                operationCode = 21;
                operand = dataCounter--;
                smlArray[instructionCounter++] = getCompleteInstruction();
                compilerStack.push(operand);
                break;

           }

         }
      }
     tokenPtr = strtok(NULL," ");
    }

}

//--------------------------------------------------------------------------------
int Compiler::isInSymbolTable(int symbol,char type)
{
   for(int i = 0 ; i < symbolTableCounter ; i++)
       if(symbolTable[i].symbol == symbol && symbolTable[i].type == type)
                return i+1;
   return 0;
}
//--------------------------------------------------------------------------------
int Compiler::getSymbolLocation(int symbol,char type)
{
  int loc = isInSymbolTable(symbol,type);
  if(loc)
     return symbolTable[loc - 1].location;
  return loc;
}

//--------------------------------------------------------------------------------
int isIntegerString(const char *str)
{
  for(int i = 0; i < strlen(str) ; i++)
    if(isdigit(str[i]))
      continue;
    else
      return 0;
  return 1;
}

//--------------------------------------------------------------------------------
#endif