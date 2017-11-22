#pragma once

#include <sstream>
#include <typeinfo>
#include <cxxabi.h>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <sys/time.h> // non-portable..

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

namespace io {

    enum class loglevel : uint8_t {
        quiet = 0,
        error,
        warn,
        info,
        all,
        none
    };

    class logstream : public std::ostringstream {
        std::ostream& _out;
        std::string _dtstring;
        loglevel _curlevel;
    public:
        logstream(std::ostream& out) : _out(out), _dtstring("%Y%m%d %H:%M:%S") {
        }

        virtual ~logstream() {
            flush();
        }

        void flush() {
            _out << str();
            _out.flush();
            str("");
            _curlevel = loglevel::none;
        }
        /**
        template <typename S> 
        logstream& operator << (const S& t) {
            (*reinterpret_cast<std::ostringstream*>(this)) << t;
            return *this;
        }*/

        template <typename T>
        inline logstream & operator<<( const T & t )
        {
            (*(std::ostringstream*)this) << t;
            return *this;
        }

        /**
        logstream& operator << (const loglevel& l) {
            _curlevel = l;
            return *this;
        }*/

        typedef logstream & (*logstream_manip)(logstream &);
        logstream & operator<<(logstream_manip manip) { return manip(*this); }
    };

        template <typename R>
        static std::string value_of(const std::string& name, const R& val, bool print_type=false) {
            std::stringstream ss;
            ss << RESET << name;
            if(print_type) {
                int status = 0;
                auto ti = typeid(val).name();
                auto realname = abi::__cxa_demangle(ti, 0, 0, &status);
                ss << " (" << WHITE << realname << RESET << ")";
            }
            ss << "=[" << BLUE << val << RESET << "]";
            return ss.str();
        }


    //logstream& operator << (logstream& out, logstream::level lvl);
#define DEBUG_METHOD()              io::logstream __logstream(std::cout)
#define DEBUG_VALUE_OF(x)           __logstream << io::value_of(#x, x, false) << std::endl
#define DEBUG_VALUE_AND_TYPE_OF(x)  __logstream << io::value_of(#x, x, true) << std::endl
}
namespace std { 
    io::logstream & endl(io::logstream & out) { 
        out.put('\n'); 
        out.flush(); 
        return out; 
    } 
}
