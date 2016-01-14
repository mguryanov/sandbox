#include <iostream>
#include <vector>

using namespace std;

int main()
{
    vector<char> stack;

    char c;
    while( cin.get(c) ) {
        if( c == '\n' )
            continue;
        stack.push_back( c );
    }

    vector<char>::const_reverse_iterator it=stack.crbegin();

    for( ; it != stack.crend() ; ++it ) {
        cout << *it;
    }

    cout << endl;
    return 0;
}
