#include <iostream>
#include <vector>
#include <map>

using namespace std;


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

        if ( n > 100000 ) {
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

    string line;
    getline (cin,line);

    if ( cin.fail() ) {
        return false;
    }

    string illegal_characters("\\,|<>:#$%{}()[]\'\"^!?+*");
    size_t pos = line.find_first_of(illegal_characters,0);
    if( pos!=string::npos ) {
        return false;
    }

    string::iterator it=line.begin();

    for ( ; it!=line.end(); ++it ) {

        if ( *it == ' ' && !init )
            continue;

        if ( *it == ' ' ) {
            v.push_back (n*sign);
            n=0;
            init=0;
            sign=1;
            continue;
        }

        if ( *it == '-' && !init ) {
            sign=-1;
            continue;
        }

        init=1;
        n*=10;
        n+=(*it-'0');

        if ( n > 100000 ) {
            return false;
        }
    }

    if ( init ) {
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
                 max_j=0,
                 max_ij=0;

    int max_sum=v1[0]+v2[0];

    for ( ; j < matrix_size; ++j, ++i ) {

        if ( v1[max_i] < v1[i] ) {
            max_i=i;
        }

        if ( v2[max_j] < v2[j] ) {
            if ( v1[max_ij] < v1[max_i] ) {
                max_ij=max_i;
            }
            max_j=j;
            max_sum=v1[max_ij]+v2[max_j];
        }

        else {
            if ( v1[max_ij] < v1[max_i] ) {
                int curr_sum=v1[max_i]+v2[j];
                if ( max_sum < curr_sum ) {
                    max_sum=curr_sum;
                    max_ij=max_i;
                    max_j=j;
                }
            }
        }
    }

    cout << max_ij << " " << max_j;
    return 0;
}
