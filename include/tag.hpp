//
// All the defined constants and functions relevant to tags, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//

#ifndef TAG_HPP_
#define TAG_HPP_

#include <string>

#include "CLI11.hpp"

namespace tm_tag {

    // Descriptions of all the possible flags and subcommands
    std::string ADD_DESCRIPTION = "add a tag to the list of possible tags";
    std::string COLOR_DESCRIPTION =
        "specifies the color for the task, must be an ANSI compliant color";
    std::string RM_DESCRIPTION = 
        "remove a an existing tag, will fail if tag is used by any tasks";
    std::string NAME_DESCRIPTION =
        "required flag to specify the name of the tag being modified";
    std::string HARD_RM_DESCRIPTION =
        "remove the tag even if it other tasks use it";
    std::string LIST_DESCRIPTION =
        "display the current tags used";
    std::string MAX_DESCRIPTION =
        "the maximum number of tags to be displayed, by default, there is no max";
    std::string NOCOLOR_DESCRIPTION =
        "do not display the colors of the tags, usefull if output is directed to a file";

    // functions for handling the different tag subcommands
    void handle_rm(const std::string &tag_name, bool hard=false);
    void handle_add(const std::string &tag_name);
    void handle_list(bool nocolor, int max_tags);

    // class for defining colors
    class TagColor {
        TagColor(std::string color);
        std::string format_string(std::string input);
    };

}

#endif // TAG_HPP_
