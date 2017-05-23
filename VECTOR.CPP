#include<iostream.h>
#include<assert.h>
template <class T> class vector
{
public:
//constructorsand destructor
vector(unsigned int n);
vector(unsigned int n, T &value);
vector(const vector &source);
~vector();

T & operator[](unsigned int index) const;
void operator=(const vector&);

unsigned int length() const;
unsigned int setSize(int n);
unsigned int setSize(unsigned int n,T &value);

friend ostream& operator<<
(ostream& theStream,vector &v);
friend istream& operator>>
(istream& theStream,vector &v);
protected:
T * data;
unsigned int size;
};



template <class T> vector<T>::vector(unsigned int n):size(n)
{
data=new T[size];
assert(data!=0);
}


template <class T> vector<T>::vector(unsigned int n,T &value):size(n)
{
data=new T[size];
assert(data!=0);
for(int i=0;i<size;i++)
data[i]=value;
}

template <class T> vector<T>::vector(const vector<T> &source):size(source.size)
{
data=new T[size];
assert(data!=0);
for(int i=0;i<size;i++)
data[i]=source.data[i];
}


template <class T> vector<T>::~vector()
{
delete [] data;
data=0;
size=0;
}


template <class T> T & vector<T>::operator[](unsigned int index) const
{
assert(index<size);
return data[index];
}

template <class T> void vector<T>::operator=(const vector<T> &source)
{
for(int i=0;i<size;i++)
data[i]=source.data[i];
}

template <class T> unsigned int vector<T>::length() const
{
return size;
}


template <class T> unsigned int vector<T>::setSize(unsigned int n,T &value)
{
T *newData=new T[n];
assert(newData!=0);
if(n<=size){
for(int i=0;i<n;i++)
newData[i]=data[i];
}
else {
for(int i=0;i<size;i++)
newData[i]=data[i];
for(i=size;i<n;i++)
newData[i]=value;
}
delete [] data;
size=n;
data=newData;
return size;
}


template <class T>unsigned int vector<T>::setSize(int n)
{
T *newData=new T[n];
assert(newData!=0);
if(n<=size)
{
for(int i=0;i<n;i++)
newData[i]=data[i];
}
else {
for(int i=0;i<size;i++)
newData[i]=data[i];
}
delete [] data;
size=n;
data=newData;
return size;
}


ostream& operator<<(ostream &out,vector<T> &v)
{
int size=v.size;
out<<"[ ";
for(int i=0;i<size;i++)
out<<v.data[i]<<" ";
out<<" ]";
return out;
}


istream& operator>>(istream &in,vector<T> &v)
{
int size=v.size;
for(int i=0;i<size;i++)
in>>v.data[i];
return in;
}

