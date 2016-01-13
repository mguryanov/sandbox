#include <iostream>
#include <vector>

using namespace std;

static const unsigned int num_limit=100000;



bool
get_size_from_stdin( size_t* s )
{
    int n=0;
    char c;

    while(cin.get(c)) {
        if (c == '\n' ) {
            *s = n;
            break;
        }

        if ( n == 0 && (c == '-' || c == ' ' )) {
            continue;
        }

        n*=10;
        n+=(c-'0');

        if ( n > num_limit ) {
            return false;
        }
    }

    return true;
}



bool
get_vector_from_stdin( vector<int>& v )
{
    int n=0;
    int sign=1;
    char c;

    while(cin.get(c)) {
        if (c == '\n' ) {
            v.push_back (n);
            break;
        }

        if ( c == ' ' ) {
            v.push_back (n*sign);
            n=0;
            sign=1;
            continue;
        }

        if ( n == 0 && c == '-' ) {
            sign=-1;
            continue;
        }

        n*=10;
        n+=(c-'0');

        if ( n > num_limit ) {
            return false;
        }
    }

    return true;
}



int main()
{
    size_t size;
    vector<int> v1;
    vector<int> v2;

    if ( !get_size_from_stdin( &size ) || size==0 || size > num_limit ||
         !get_vector_from_stdin( v1 )  ||
         !get_vector_from_stdin( v2 ) ) {
        return 0;
    }

    unsigned int i=0,
                 j=1;

    unsigned int max_i=0,
                 max_j=0;

    ssize_t max_sum=v1[0]+v2[0];

    for ( ; i < size; ++i,j=i ) {
        for ( ; j < size; ++j ) {
            ssize_t curr_sum=v1[i]+v2[j];
            if ( max_sum < curr_sum ) {
                max_sum = curr_sum;
                max_i=i;
                max_j=j;
            }
        }
    }

    cout << max_i << ' ' << max_j;
    return 0;
}
