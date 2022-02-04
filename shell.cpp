#include "shell.h"
#include <sstream>

using namespace cplus;

shell::shell() : l(*this), p(l, *this) { }

int shell::parse() {
    return p.parse();
}

void shell::prompt() {
    if((*this).interactive) {std:: cout << "cplus> ";}
}

void shell::readFrom(std::istream *is) {
    l.switch_streams(is, NULL);  
}
