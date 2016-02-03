#include "test-html-entities.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <deque>

using namespace std;
using namespace HPHP;


bool
test_encode_sentence(
        const char* sentence,
        entity_charset charset,
        HPHP::EntBitmask doc )
{
    int64_t flag=static_cast<int64_t>( doc )                        |
                 static_cast<int64_t>( EntBitmask::ENT_BM_IGNORE )  |
                 static_cast<int64_t>( EntBitmask::ENT_BM_SINGLE )  |
                 static_cast<int64_t>( EntBitmask::ENT_BM_DOUBLE );

    int len=std::char_traits<char>::length( sentence );
    char* encoded_response=HPHP::string_html_encode(
                                        sentence,
                                        len,flag,
                                        charset,
                                        true,
                                        true
                                   );

    cout << "\033[1;32m"
         << encoded_response
         << "\033[0m" << endl;

    return true;
}


bool
test_encode_entities(
        const char* header,
        pair<const char*,test_case_meta_t>* row,
        vector<pair<const char*,const char*>>& result,
        deque<pair<size_t,size_t>>& pos,
        bool all)
{
    cout << header << row->first << " ";

    int64_t flag=static_cast<int64_t>( row->second.doc )            |
                 static_cast<int64_t>( EntBitmask::ENT_BM_SINGLE ) |
                 static_cast<int64_t>( EntBitmask::ENT_BM_DOUBLE );

    entity_test_case_t* table=
            reinterpret_cast<entity_test_case_t*>( row->second.ptr );

    size_t begin = result.size();
    size_t end = begin;

    for( auto v : *table )
    {
        if( !v.second )
            continue;

        int len=std::char_traits<char>::length( v.first );
        char* encoded_response=HPHP::string_html_encode(
                                            v.first,len,flag,
                                            row->second.charset,
                                            true,
                                            all
                                       );

        int offset=( len == 1 ) ? 0 : 1;
        len=std::char_traits<char>::length( v.second );

        if( strncmp( encoded_response+offset, v.second, len ) != 0 ) {
            if( all ) {
                cout << setfill(' ') << setw(25) << "\033[1;31m" <<
                        encoded_response << "\t" <<
                        v.second << "\033[0m" << endl;
                return false;
            }
        }

        else {
            result.push_back(
                        pair<const char*,const char*>(
                                encoded_response,
                                v.first
                            )
                    );
            ++end;
        }
    }

    pos.push_back( pair<size_t,size_t>( begin, --end ));
    cout << "\033[1;32mOk!\033[0m" << endl;

    return true;
}



bool
test_decode_entities(
        const char* header,
        pair<const char*,test_case_meta_t>* row,
        vector<pair<const char*,const char*>>& result,
        deque<pair<size_t,size_t>>& pos,
        bool all)
{
    cout << header << row->first << " ";

    int64_t flag=static_cast<int64_t>( row->second.doc )           |
                 static_cast<int64_t>( EntBitmask::ENT_BM_SINGLE ) |
                 static_cast<int64_t>( EntBitmask::ENT_BM_DOUBLE );

    bool bad_case=false;
    pair<size_t,size_t> p=pos[0];
    pos.pop_front();

    for( int i = p.first; i <= p.second; ++i )
    {
        int len=std::char_traits<char>::length( result[i].first );
        char* decoded_response=HPHP::string_html_decode(
                                            result[i].first,len,flag,
                                            row->second.hint_charset,
                                            all
                                       );

        len=std::char_traits<char>::length( result[i].second );

        if( strncmp( decoded_response, result[i].second, len ) != 0 ) {
            bad_case=true;
            cout << setfill(' ') << setw(25) << "\033[1;31m" <<
                    decoded_response << "\t" <<
                    result[i].second << "\033[0m" << endl;
        }

        free( decoded_response );

        if( bad_case ) {
            return false;
        }
    }

    cout << "\033[1;32mOk!\033[0m" << endl;
    return true;
}


void __free(vector<pair<const char*,const char*>>& result)
{
    for( auto inst : result ) {
        free( const_cast<char*>( inst.first ) );
    }

    return;
}



int main()
{
     vector<pair<const char*,const char*>> result;
     deque<pair<size_t,size_t>> pos;

     // htmlentities

    for( auto v : test_single_cases ) {
        if( test_encode_entities(
                "htmlentities single encode: ",
                &v, result, pos, true
            ) == false )
        {
            __free( result );
        }
    }

    for( auto v : test_single_cases ) {
        if( test_decode_entities(
                "htmlentities single decode: ",
                &v, result, pos, true
            ) == false )
        {
            __free( result );
        }
    }

    // htmlspecialchars

    result.clear();
    pos.clear();

    for( auto v : test_single_cases ) {
        if( test_encode_entities(
                "htmlspecialchars single encode: ",
                &v, result, pos, false
            ) == false )
        {
            __free( result );
        }
    }

    for( auto v : test_single_cases ) {
        if( test_decode_entities(
                "htmlspecialchars single decode: ",
                &v, result, pos, false
            ) == false )
        {
            __free( result );
        }
    }

    // text slot test

    // HTML401

    test_encode_sentence(
                koi8r_sentence,
                cs_koi8r,
                EntBitmask::ENT_BM_HTML401
            );

    test_encode_sentence(
                cp866_sentence,
                cs_cp866,
                EntBitmask::ENT_BM_HTML401
            );

    test_encode_sentence(
                cp1251_sentence,
                cs_cp1251,
                EntBitmask::ENT_BM_HTML401
            );

    test_encode_sentence(
                utf8_sentence,
                cs_utf_8,
                EntBitmask::ENT_BM_HTML401
            );

    test_encode_sentence(
                koi8r_sentence,
                cs_koi8r,
                EntBitmask::ENT_BM_HTML5
            );

    // HTML5

    test_encode_sentence(
                cp866_sentence,
                cs_cp866,
                EntBitmask::ENT_BM_HTML5
            );


    test_encode_sentence(
                cp1251_sentence,
                cs_cp1251,
                EntBitmask::ENT_BM_HTML5
            );

    test_encode_sentence(
                utf8_sentence,
                cs_utf_8,
                EntBitmask::ENT_BM_HTML5
            );

    return 0;
}
