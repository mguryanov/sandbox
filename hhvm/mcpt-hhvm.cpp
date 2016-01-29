#include "html-table.h"
#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;

int
main (void)
{
    auto native_html5_tbl = HPHP::get_entity_table_html5();

    for ( auto& row : native_html5_tbl ) {
        cout << " code : " << row.first << endl;
        const HPHP::entity_row& ent=row.second;
        if ( ent.entity!=nullptr ) {
            cout << "\tname : " << ent.entity << endl;
        } else {
            auto mc_row_iter=ent.multicodepoint_table->begin();
            cout << "\trows num : " << (*mc_row_iter).second_codepoint << endl;
            if ( (*mc_row_iter).entity!=nullptr ) {
                cout << "\tdefault name : " << (*mc_row_iter).entity << endl;
            }
            else {
                cout << "\tdefault name : none " << endl;
            }

            ++mc_row_iter;

            for ( ; mc_row_iter != ent.multicodepoint_table->end(); ++mc_row_iter ) {
                cout << "\tsecond code : " << (*mc_row_iter).second_codepoint << endl;
                if ( (*mc_row_iter).entity!=nullptr ) {
                    cout << "\tname : " << (*mc_row_iter).entity << endl;
                }
                else {
                    cout << "\tname : none " << endl;
                }
            }
        }
    }

    return 0;
}
