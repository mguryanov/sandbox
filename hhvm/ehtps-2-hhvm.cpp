#include "zend-html.h"
#include "test-html-entities.h"
#include <iostream>
#include <iomanip>

using namespace std;

int main()
{
    int64_t flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_XHTML );
    cout << "htmlentities: cp1251_xhtml " << endl;
/*
    for( auto v : cp1251_html401 )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_cp1251,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }

    flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_HTML401 );
    cout << "htmlentities: cp1251_html401 " << endl;

    for( auto v : cp1251_html401 )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_cp1251,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }

    flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_HTML5 );
    cout << "htmlentities: cp1251_html5 " << endl;

    for( auto v : cp1251_html5 )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_cp1251,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }
    flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_XHTML );
    cout << "htmlentities: utf8_xhtml " << endl;

    for( auto v : utf8_xhtml )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_utf_8,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }

    flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_HTML401 );
    cout << "htmlentities: utf8_html401 " << endl;

    for( auto v : utf8_html401 )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_utf_8,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }
*/

    flag=static_cast<int64_t>( HPHP::EntBitmask::ENT_BM_HTML5 );
    cout << "htmlentities: utf8_html5 " << endl;

    for( auto v : utf8_html5 )
    {
        int len=sizeof( v.first );
        char* response_htmlentities=HPHP::string_html_encode(
                                                v.first,len,flag,
                                                entity_charset::cs_utf_8,
                                                true,
                                                true
                                           );

        cout << setfill(' ') << setw(25) <<
                response_htmlentities << "\t" <<
                v.second << endl;

        free( response_htmlentities );
    }


//    char* response_htmlspecialchars=HPHP::string_html_encode(
//                                                   str,len,flag,
//                                                   entity_charset::cs_cp1251,
//                                                   true,
//                                                   false
//                                               );

    return 0;
}
