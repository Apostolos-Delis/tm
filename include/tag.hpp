//
// All the defined constants and functions relevant to tags, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//

#ifndef TAG_HPP_
#define TAG_HPP_

#include <string>
#include <ostream>

#include "database.hpp"

namespace tm_tag {

    // Descriptions of all the possible flags and subcommands
    const std::string ADD_DESCRIPTION = 
        "add a tag to the list of possible tags, updates the tag's color if the tag exists";
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

    /**
     * removes a tag from the database
     * @param[in] tag_name: a string of the tag specific task
     * @param[in] hard: a bool representing whether to hard remove a tag, this means
     * that even if other tasks used the tag, it would still be removed
     */
    void handle_rm(const std::string &tag_name, bool hard=false);

    /**
     * Inserts a tag into the tags table
     * @param[in] tag_name: a string of the tag specific task
     * @param[in] color: a color like "red", this will be converted into a
     * valid ansi color code if the color is valid
     */
    void handle_add(const std::string &tag_name, std::string color);


    /**
     * displays a list of the tags from the database
     * @param[in] tag_name: a string of the tag specific task
     * @param[in] color: a color like "red", this will be converted into a
     * valid ansi color code if the color is valid
     */
    void handle_list(bool no_color, int max_tags);

}

#endif // TAG_HPP_
