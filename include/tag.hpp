//
// All the defined constants and functions relevant to tags, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//

#ifndef TAG_HPP_
#define TAG_HPP_

#include <string>
#include <ostream>


namespace tm_tag {

    // Descriptions of all the possible flags and subcommands
    const std::string ADD_DESCRIPTION = "add a tag to the list of possible tags";
    const std::string COLOR_DESCRIPTION =
        "specifies the color for the task, must be an ANSI compliant color";
    const std::string RM_DESCRIPTION = 
        "remove a an existing tag, will fail if tag is used by any tasks";
    const std::string NAME_DESCRIPTION =
        "required flag to specify the name of the tag being modified";
    const std::string HARD_RM_DESCRIPTION =
        "remove the tag even if it other tasks use it";
    const std::string LIST_DESCRIPTION =
        "display the current tags used";
    const std::string MAX_DESCRIPTION =
        "the maximum number of tags to be displayed, by default, there is no max";
    const std::string NOCOLOR_DESCRIPTION =
        "do not display the colors of the tags, usefull if output is directed to a file";

    // functions for handling the different tag subcommands
    void handle_rm(const std::string &tag_name, bool hard=false);
    void handle_add(const std::string &tag_name, std::string color);
    void handle_list(bool no_color, int max_tags);


}
// class for defining colors
namespace tm_color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_WHITE    = 47,
    };
    class Color {
    private:
        Code code_;
    public:
        Color(Code pCode) : code_(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Color& mod) {
            return os << "\033[" << mod.code_ << "m";
        }
    };
}

#endif // TAG_HPP_
