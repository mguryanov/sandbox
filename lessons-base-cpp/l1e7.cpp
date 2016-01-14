#include <iostream>
using namespace std;
int i = 2;
class Test
{
public:
  int i,j;
  Test(int i = 0) {this->i = i+1; j=i;}
};

int main()
{
  Test i, j(1);
  cout << i.i<<j.i<<i.j<<j.j<< endl;
}
