#include <iostream>

using namespace std;


class Matherr {

    public:
        virtual string what() const {
            return string("base");
        }
};



class DerivedEx : public Matherr {

        int op1_;
        int op2_;

    public:
        DerivedEx (int first,int second)
            :
              op1_ (first),
              op2_ (second)
        {}

        ~DerivedEx () {
            cout << __FUNCTION__ << endl;

            if (uncaught_exception()) {
                cout << "uncaught exception == true" << endl;
            }
        }

        string what () const {
            cout << __FUNCTION__ << endl;
            return string("derived");
        }

};



struct Instance {

        Instance ()
            :
              size_ (0)
        {}

        void set (size_t s) {
            size_=s;
        }

        size_t get () {
            return size_;
        }

        void _throw () throw (Matherr) {
            throw Matherr();
        }

        ~Instance () throw (Matherr) {
            cout << __FUNCTION__ << endl;

            if (uncaught_exception()) {
                cout << "uncaught exception == true" << endl;
                return;
            }

            try {
                throw Matherr();
            }

            catch (...) {
                return;
            }
        }

    private:
        size_t size_;
};





int
main (void)
{
    try {
        Instance in;
//        in._throw ();
//        throw DerivedEx(1,2);
    }

    catch (Matherr e) {
        /*  match : cut-down */
        cout << __FUNCTION__ << endl;
        cout << e.what () << endl;
    }

    catch (const Matherr& e) {
        /* miss */
        cout << e.what () << endl;
    }

    catch (DerivedEx& e) {
        /* miss */
        cout << e.what () << endl;
    }

    catch (...) {
        /* unreached */
    }

    return 1;
}
