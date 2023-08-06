#include <iostream>
using namespace std;
class A
{
    long long m;
};
int main()
{
    int **p2;
    int a = 3;
    int *p1 = &a;

    *p2 = &a;
    cout << **p2 << endl;
}