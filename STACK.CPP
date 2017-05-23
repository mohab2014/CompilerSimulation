#include<iostream.h>
#include"vector.hpp"
class stack
{
public:
stack(int size);
stack(const stack &s);
void deleteAllValues();
int isEmpty() const;
int pop();
void push(int value);
int top() const;
protected:
vector data;
int nextSlot;
};
stack::stack(int size):data(size)
{
deleteAllValues();
}
stack::stack(const stack &s):data(s.data),nextSlot(s.nextSlot)
{
}
