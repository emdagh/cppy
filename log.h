#pragma once

#include <sstream>
#include <typeinfo>
#include <cxxabi.h>
#include <iterator>
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

    std::ostream& red(std::ostream& os) {
        os << RED;
        return os;
    }


    std::ostream& reset(std::ostream& os) {
        os << RESET;
        return os;
    }

    template <typename T, typename S=std::char_traits<T> >
    class basic_logstreambuf : public std::basic_streambuf<T, S> {
        typedef S traits;
        typedef typename traits::int_type int_type;
        typedef std::basic_string<T, S> string_type;

        //string_type pbuf;
        //static const size_t BUFFER_SIZE=32;
        //T pbuf[BUFFER_SIZE];

        string_type pbuf;
        string_type context;
        string_type dtformat;

        std::basic_streambuf<T, S>* sb;
        typedef std::basic_streambuf<T, S> super_type;
        using super_type::pbase;
        using super_type::pbump;
        using super_type::pptr;
        using super_type::epptr;
        using super_type::setp;
		
		std::string now() {
    		timeval curTime;
		    gettimeofday(&curTime, NULL);
	    	int milli = curTime.tv_usec / 1000;
	
    		char buffer [80];
		    strftime(buffer, 80, dtformat.c_str(), localtime(&curTime.tv_sec));

	    	char currentTime[84] = "";
    		sprintf(currentTime, "%s:%3d", buffer, milli);

		    return std::string(currentTime);
        }
        

    protected:

        int stamp_and_flush(char* buf, int n) {

            std::stringstream ss;
            ss << "[" << MAGENTA << now() << RESET << "] " << context << "> ";
            auto timestr = ss.str();
            sb->sputn(&timestr.front(), timestr.length());
            sb->sputn(buf, n); //&pbuf.front(), pbuf.size());

        }
        // overflow just grows the buffer in this case.
        virtual int_type overflow(int_type c=traits::eof()) {
            auto ch = traits::to_char_type(c);
            auto n = pptr() - pbase();
            pbuf.push_back(ch);
            setp(&pbuf.front(), &pbuf.back() + 1);
            pbump(n + 1);
            return traits::not_eof(ch);
        }


        virtual int sync() {
            if(pptr() + 1 >= epptr()) {
                pbuf.push_back(static_cast<T>(0));
                setp(&pbuf.front(), &pbuf.back() + 1);
            } else {
                *pptr() = static_cast<T>(0);
            }

            auto n = pptr() - pbase();

            stamp_and_flush(&pbuf.front(), pbuf.size());
            
            return 0;
        }

    public:
        basic_logstreambuf(const string_type& context, std::basic_streambuf<T, S>* out) 
        : sb(out)
        , context(context)
        , dtformat("%Y-%m-%d %H:%M:%S")
        {
            setp(&pbuf.front(), &pbuf.back() + 1);
        }

        virtual ~basic_logstreambuf() {
            if(pptr() != &pbuf.front()) {
                sync();
            }
        }

        void set_context(const string_type& c) {
            context = c;
        }
    };

    typedef basic_logstreambuf<char> logstreambuf;


    template <typename T, typename S=std::char_traits<T> >
    class basic_logstream : public std::basic_ostream<T, S> {
        typedef basic_logstreambuf<T, S> buffer_type;
        typedef std::basic_ostream<T, S> super_type;
        buffer_type buf;
        //
        //super_type& out;
    public:
        basic_logstream(const std::string& context, super_type& os) 
        //: out(os)
        : super_type(&buf)
        , buf(context, os.rdbuf()) 
        {
            this->write("enter\n", 7);
            buf.pubsync();
        }  

        virtual ~basic_logstream() {
        }

        /*template <typename R>
        void value_of(const R& val, bool has_type) {
        }*/
    public:


        template <typename R>
        static std::string value_of(const std::string& name, const R& val, bool print_type=false) {
            std::stringstream ss;
            ss << io::reset << name;
            if(print_type) {
                int status = 0;
                auto ti = typeid(val).name();
                auto realname = abi::__cxa_demangle(ti, 0, 0, &status);
                ss << " (" << WHITE << realname << RESET << ")";
            }
            ss << "=[" << BLUE << val << RESET << "]";
            return ss.str();
        }
    };

    typedef basic_logstream<char> logstream;
#define DEBUG_METHOD()              io::logstream __logstream(__FUNCTION__, std::cout)
#define DEBUG_VALUE_OF(x)           __logstream << io::logstream::value_of(#x, x, false) << std::endl;
#define DEBUG_VALUE_AND_TYPE_OF(x)  __logstream << io::logstream::value_of(#x, x, true) << std::endl;
}
