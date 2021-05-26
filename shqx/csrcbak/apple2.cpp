#include<iostream> 
using namespace std; 

class Apple 
{ 
public: 
    static int i; 

    Apple() 
    { 
        // Do nothing 
    }; 
}; 

int Apple::i = 1; 

int main() 
{ 
    Apple obj; 
    Apple obj1;
    
    //obj1.i++;
    // prints value of i 
    cout << obj.i<<endl; 
} 
