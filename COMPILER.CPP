/*******************************************************************************
*********************** Author: Mohammed Ahmed Awad Elkariem **************************
***************************Faculty of Mathematical Sciences ***************************
********************************** MAIN PROJECT FILE ********************************
**************************** COMPILER TO THE SML LANGUAGE ***********************************
*********************************************************************************************/

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
// private:
   int instructionCounter;
   int dataCounter;
   int operationCode;
   int operand;
   int smlArray[100];
   TableEntry symbolTable[100];
   int symbolTableCounter;
   const char* simpleCommand[COMMANDSNUMBER];
   Stack<int> compilerStack;
   Stack<char> toPostfixStack;

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
  simpleCommand[6] = "end";

  instructionCounter = 0;
  symbolTableCounter = 0;
  dataCounter = 99;

  for(int i = 0; i < 100 ; i++)
    smlArray[i] = 0;
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
       else if(!strcmp(tokenPtr,"let"))
       {
          int letVariable;
          tokenPtr = strtok(NULL," ");
          if(strlen(tokenPtr) == 1 && isalpha(tokenPtr[0]))
          {
            if(!isInSymbolTable(toascii(tokenPtr[0]),'v'))
            {
               symbolTable[symbolTableCounter++].insertSymbol(toascii(tokenPtr[0]),'v',letVariable = dataCounter);
               smlArray[dataCounter--] = 0;
            }
            else
               letVariable = getSymbolLocation(toascii(tokenPtr[0]),'v');
          }
          else
	  {
	    cout<<"\nERROR. After \"let\" command there must be a variable(single character) ";
	    exit(1);
	  }

	  tokenPtr = strtok(NULL," ");
	  char *infix = "";
	  char *postfix1,*postfix2;

	  if(strlen(tokenPtr) == 1 && tokenPtr[0] == '=')
	  {
	      while(tokenPtr != NULL)
	      {
		  tokenPtr = strtok(NULL," ");
		  infix = strcat(infix,tokenPtr);
	      }
	      //append ) to infix
	      infix = strcat(infix,")");
	      cout<<"infix = "<<infix<<"\n";

	      postfix2 = convertToPostfix(toPostfixStack,infix);
	      insertLetSymbols(postfix2);
	      delete [] postfix2;

	      // empty stack
	      int eat;
	      while(!toPostfixStack.isStackEmpty())
	      {
		toPostfixStack.pop(eat);
	      }
	      cout<<"\ninfix = "<<infix;

	      postfix1 = convertToPostfix(toPostfixStack,infix);
	      cout<<"\npostfix = "<<postfix1;
	      evaluatePostfixExpression(postfix1);
	      delete [] postfix1;

	      // pop the result from stack and assign it to let stmt.
	      operationCode = 20;//LOAD
	      compilerStack.pop(operand);

              smlArray[instructionCounter++] = getCompleteInstruction();
              // store  to let Variable
              operationCode = 21;//STORE
              cout<<"\nlet = "<<letVariable;
              operand = letVariable;
              smlArray[instructionCounter++]  = getCompleteInstruction();
          }

          else
          {
             cout<<"\nERROR. assignment(=) expected";
	     exit(1);
          }

       }
       else if(!strcmp(tokenPtr,"if"))
       {
       }
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
	 {
            symbolTable[symbolTableCounter].insertSymbol(atoi(tokenPtr),'c',dataCounter);
	    smlArray[dataCounter--] = atoi(tokenPtr);
         }
      else if(strlen(tokenPtr) == 1)
      {
         if(isalpha(tokenPtr[0]))       // variable
            if(!isInSymbolTable(atoi(tokenPtr),'v'))
            {
                 symbolTable[symbolTableCounter++].insertSymbol(toascii(tokenPtr[0]),'v',dataCounter);
                 smlArray[dataCounter--] = 0;  // initialize with ZERO
            }
         else
           ;// it is operator,skip it
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

   for(int i = 0 ; i <= symbolTableCounter ; i++)
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


//---------------------------------------------------------------------------
int main()
{
  Compiler smlCompiler;
  char* stmt = "10 let x = 2 * 4";
  smlCompiler.compileStatement(stmt);
/*  int i;
  for(i = 0;i < 5;i++)
    cout<<"\n"<<smlCompiler.smlArray[i];
  for(i = 0; i < 3;i++)
    cout<<"\n"<<smlCompiler.symbolTable[i].location<<"\t"<<
		smlCompiler.symbolTable[i].symbol;    */
  return 0;
}
//---------------------------------------------------------------------------
