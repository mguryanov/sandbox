#include <iostream>

using namespace std;

bool is_prime( int n )
{
    if ( n==1 || n%2==0 ) {
        return false;
    }

    for ( int i=2; i*i<=n; ++i ) {
        if ( n % i == 0 ) {
            return false;
        }
    }

    return true;
}



int main()
{
    int n=0;
    char c;

    while(cin.get(c)) {
        if (c == '\n' )
            break;
        n*=10;
        n+=(c-'0');
    }

    for ( int i=2; i<=n; ) {

        if ( is_prime( n ) ) {
            cout << n << ' ';
            break;
        }

        if ( n % i == 0 ) {
            n/=i;
            cout << i << ' ';
            continue;
        }

        ++i;
    }

  return 0;
}
