
// Parser Class definition, returns a p
//
//
//

#ifndef Parser_HPP_
#define Parser_HPP_

#include "CLI11.hpp"


namespace parser {

    class ArgParse {
    private:
    public:
        ArgParse();
        void parse_args(int argc, char** argv);
    };
}

#endif // Parser_HPP_
