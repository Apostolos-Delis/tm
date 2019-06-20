//
// Implementations of the different subroutines for handling sess commands
//

#include <string>
#include <vector>
#include <iostream>

#include "sess.hpp"

// Start a session with the specified attributes
void tm_sess::handle_start(double length, bool no_interupt, bool no_overtime,
                           const std::string &task,
                           const std::string &description) {
    std::cout << "length: " << length << " task: " << task << std::endl;
}


// Display a log of the most recent sessions
void tm_sess::handle_log(bool condensed, int max_sessions) {

}
