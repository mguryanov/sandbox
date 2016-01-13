#include <iostream>
#include <limits>
#include <new>

using namespace std;

int
main (int argc,
      char** argv)
{
    double* data=nullptr;

    try {
        int max=std::numeric_limits<int>::max();
        data=new double[max];
    }

    catch (bad_alloc& e) {
        cout << e.what () << endl;
    }

    delete[] data;

    return 0;
}

