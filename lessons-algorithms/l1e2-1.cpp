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

    if (cin.eof())
        *s = n;

    return true;
}



bool
get_vector_from_stdin( vector<int>& v )
{
    int n=0;
    int init=0;
    int sign=1;
    char c;

    while(cin.get(c)) {

        if ( c == ' ' && !init )
            continue;

        if ( c == '\n' ) {
            if ( init )
                v.push_back (n*sign);
            break;
        }

        if ( c == ' ' ) {
            v.push_back (n*sign);
            n=0;
            init=0;
            sign=1;
            continue;
        }

        if ( c == '-' && !init ) {
            sign=-1;
            continue;
        }

        if ( c > '9' && c < '0' ) {
            return false;
        }

        init=1;
        n*=10;
        n+=(c-'0');

//        cout << ": c : " << c;
//        cout << ": n : " << n;

        if ( n > num_limit ) {
            return false;
        }
    }

    if (cin.eof()) {
        v.push_back (n*sign);
    }

    return true;
}



int main()
{
    size_t matrix_size;
    vector<int> v1;
    vector<int> v2;

    if ( !get_size_from_stdin( &matrix_size ) || matrix_size==0 ||
         !get_vector_from_stdin( v1 ) ||
         !get_vector_from_stdin( v2 ) ) {
        return 0;
    }


    size_t v1_size=v1.size();
    size_t v2_size=v2.size();

    if ( (matrix_size > v1_size) || (matrix_size > v2_size) ) {
        matrix_size = (v1_size > v2_size) ? v2_size:v1_size;
        return 0;
    }

    unsigned int i=1,
                 j=1;

    unsigned int max_i=0,
                 max_j=0;

    int max_sum=v1[0]+v2[0];

    for ( ; j < matrix_size; ++j, ++i ) {

        if ( v2[max_j] < v2[j] ) {
            if ( v1[max_i] < v1[i] ) {
                max_i=i;
            }
            max_j=j;
            max_sum=v1[max_i]+v2[max_j];
        }

        else {
            if ( v1[max_i] < v1[i] ) {
                int curr_sum=v1[i]+v2[j];
                if ( max_sum < curr_sum ) {
                    max_sum=curr_sum;
                    max_i=i;
                    max_j=j;
                }
            }
        }
    }

    cout << max_i << " " << max_j;
    return 0;
}
