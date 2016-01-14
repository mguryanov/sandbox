#include <cassert>
#include <iostream>
#include <cstring>
#include <iomanip>

using namespace std;

wchar_t
_tolower( wchar_t c )
{
    return ( c < 0x0430 ) ? (c+0x20):c;
}

int main()
{
    setlocale(LC_ALL,"ru_RU.UTF-8");
    unsigned first=0;
    unsigned sentence=0;

    wchar_t word_to_match[1024]={'\0'};
    wchar_t current_word[1024]={'\0'};

    wchar_t* ptr_match=word_to_match;
    wchar_t* ptr_current=current_word;

    uint sentences=0;
    uint matches=0;
    wchar_t c;

    while( wcin.get(c) ) {

        if (c == '\n' )
            break;

        if ( ( c == '?' || c == '!' || c == '.' )) {
            if( ptr_current!=current_word ) {
                *ptr_current='\0';
                if( !wcscmp( word_to_match, current_word ))
                    ++matches;
                ptr_current=current_word;
            }

            if( sentence ) {
                --sentence;
                ++sentences;
            }

            continue;
        }

        if( c == ' ' ) {
            if( !first ) {
                if( ptr_match!=word_to_match)
                    ++first;
            }

            else if( ptr_current!=current_word ) {
                *ptr_current='\0';
                if( !wcscmp( word_to_match, current_word ))
                    ++matches;
                ptr_current=current_word;
            }

            continue;
        }

        if( first ) {
            *ptr_current=_tolower(c);
            ++ptr_current;
            if( !sentence )
                ++sentence;
        }

        else {
            *ptr_match=_tolower(c);
            ++ptr_match;
        }
    }

    wcout << setprecision(2) << ((float)matches/sentences) << endl;
    return 0;
}
