#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;
typedef struct student_s student_t;
typedef struct condition_s condition_t;


struct condition_s {
    condition_s() :
        course_num(0),
        year(0)
    {}

    void set_next( const wstring& s ) {
        if( faq_name.empty() ) {
            faq_name=s;
        }
        else if( !course_num ) {
            course_num=static_cast<u_int8_t>( stoi( s ));
        }
        else {
            year=static_cast<u_int16_t>( stoi( s ));
        }
    }

    const wstring& faq() const {
        return faq_name;
    }

    u_int8_t course() const {
        return course_num;
    }

    u_int16_t byear() const {
        return year;
    }

private:
    wstring faq_name;
    u_int8_t course_num;
    u_int16_t year;
};



struct student_s {
    student_s() :
        mday(0),
        mon(0),
        year(0),
        tel_number(0),
        course_num(0),
        first(true),
        last(false)
    {}

    void set_next( const wstring& s ) {
        if( second_name.empty() ) {
            second_name=s;
            first=0;
        }
        else if( first_name.empty() )
            first_name=s;
        else if( father_name.empty() )
            father_name=s;
        else if( !mday ) {
            mday=static_cast<u_int8_t>( stoi( s ));
        }
        else if( !mon ) {
            mon=static_cast<u_int8_t>( stoi( s ));
        }
        else if( !year ) {
            year=static_cast<u_int16_t>( stoi( s ));
        }
        else if( born_place.empty() )
            born_place=s;
        else if( !tel_number ) {
            tel_number=static_cast<uint>( stoi( s ));
        }
        else if( faq_name.empty() )
            faq_name=s;
        else {
            course_num=static_cast<u_int8_t>( stoi( s ));
            last=1;
        }
    }

    const wstring& faq() const {
        return faq_name;
    }

    const wstring& fname() const {
        return first_name;
    }

    const wstring& sname() const {
        return second_name;
    }

    const wstring& fthname() const {
        return father_name;
    }

    const wstring& born() const {
        return born_place;
    }

    u_int8_t day() const {
        return mday;
    }

    u_int8_t month() const {
        return mon;
    }

    u_int8_t course() const {
        return course_num;
    }

    u_int16_t byear() const {
        return year;
    }

    uint tel() const {
        return tel_number;
    }

    bool on_begin() {
        return first;
    }

    bool is_filled() {
        return last;
    }

    void reset() {
        second_name.clear();
        first_name.clear();
        father_name.clear();
        mday=0;
        mon=0;
        year=0;
        born_place.clear();
        tel_number=0;
        faq_name.clear();
        course_num=0;
        first=true;
        last=false;
    }

private:
    wstring second_name;
    wstring first_name;
    wstring father_name;
    u_int8_t mday;
    u_int8_t mon;
    u_int16_t year;
    wstring born_place;
    uint tel_number;
    wstring faq_name;
    u_int8_t course_num;
    bool first;
    bool last;
};



bool
get_data_from_stdin( vector<student_t>& v,
                     condition_t& cond )
{
    setlocale(LC_ALL,"ru_RU.UTF-8");

    wstring line;
    getline (wcin,line);

    if ( wcin.fail() ) {
        return false;
    }

    wstring illegal_characters(L"\\|<>:#$%{}()[]\'\"^!?+*");
    size_t pos = line.find_first_of(illegal_characters,0);
    if( pos!=wstring::npos ) {
        return false;
    }

    pos = line.find_last_of(L".",line.size());
    if( pos==wstring::npos ) {
        return false;
    }

    student_t inst;
    wstring word;

    wstring::const_iterator ch=line.cbegin();

    for ( ; ch!=line.cbegin()+pos+2; ++ch ) {

        if( *ch == '\n' )
            break;

        if( *ch == ' ' )
            continue;

        if( *ch == ',' ) {
//            wcout  << "<< student >>" << word;
            inst.set_next( word );
            word.clear();
            continue;
        }

        if( *ch == '.' ) {

//            wcout  << "<< student >>" << word;
            inst.set_next( word );
            word.clear();

            if( inst.is_filled() ) {
//                wcout  << "<< student end >>" << word;
                v.push_back( inst );
                inst.reset();
            }

            continue;
        }

        word.push_back( *ch );
    }

    for ( ; ch!=line.cend(); ++ch ) {

        if( *ch == '\n' )
            break;

        if( *ch == ' ' )
            continue;

        if( *ch == ',' ) {
//            wcout  << "<< cond >>" << word;
            cond.set_next( word );
            word.clear();
            continue;
        }

        word.push_back( *ch );
    }

//    wcout  << "<< cond >>" << word;
    cond.set_next( word );
}



int main()
{
    vector<student_t> group;
    condition_t condition;
    if( !get_data_from_stdin( group, condition ))
        return 0;

    vector<student_t>::const_iterator iter=
            group.cbegin();

    int space=0;

    for( ; iter<group.cend(); ++iter ) {
        if( (*iter).faq().compare( condition.faq() ) == 0 ) {
            if ( space++ )
                wcout<< " ";
            wcout<< (*iter).sname() << ", "
                 << (*iter).fname() << ", "
                 << (*iter).fthname() << ", "
                 << setfill(L'0') << setw(2)
                 << (*iter).day() << "."
                 << setfill(L'0') << setw(2)
                 << (*iter).month() << "."
                 << (*iter).byear() << ", "
                 << (*iter).born() << ", "
                 << (*iter).tel() << ", "
                 << (*iter).faq() << ", "
                 << (*iter).course() << ".";
        }
    }

    int once=0;

    for( iter=group.cbegin(); iter<group.cend(); ++iter ) {
        if( (*iter).course() == condition.course() ) {
            if( space++ ) {
                if( !once++ )
                    wcout<< ";";
                wcout<< " ";
            }
            wcout<< (*iter).sname() << ", "
                 << (*iter).fname() << ", "
                 << (*iter).fthname() << ", "
                 << setfill(L'0') << setw(2)
                 << (*iter).day() << "."
                 << setfill(L'0') << setw(2)
                 << (*iter).month() << "."
                 << (*iter).byear() << ", "
                 << (*iter).born() << ", "
                 << (*iter).tel() << ", "
                 << (*iter).faq() << ", "
                 << (*iter).course() << ".";
        }
    }

    once=0;

    for( iter=group.cbegin(); iter<group.cend(); ++iter ) {
        if( (*iter).byear() > condition.byear() ) {
            if( space++ ) {
                if( !once++ )
                    wcout<< ";";
                wcout<< " ";
            }
            wcout<< (*iter).sname() << ", "
                 << (*iter).fname() << ", "
                 << (*iter).fthname() << ", "
                 << setfill(L'0') << setw(2)
                 << (*iter).day() << "."
                 << setfill(L'0') << setw(2)
                 << (*iter).month() << "."
                 << (*iter).byear() << ", "
                 << (*iter).born() << ", "
                 << (*iter).tel() << ", "
                 << (*iter).faq() << ", "
                 << (*iter).course() << ".";
        }
    }

    wcout << endl;


    return 0;
}
