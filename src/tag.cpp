//
// All the defined constants and functions relevant to tags, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//


#include <iostream>
#include <string>
#include <unordered_map>

#include "tag.hpp"

#define MAX_TAG_LENGTH 16


/**
 * removes a tag from the database
 * @param[in] tag_name: a string of the tag specific task
 * @param[in] hard: a bool representing whether to hard remove a tag, this means
 * that even if other tasks used the tag, it would still be removed
 */
void tm_tag::handle_rm(const std::string &tag_name, bool hard){
    auto db = tm_db::TMDatabase();
    db.remove_tag(tag_name, hard);
}


/**
 * adds a tag to the database
 * @param[in] tag_name: a string of the tag specific task, strlen must be under 
 * the defined constant MAX_TAG_LENGTH
 * @param[in] color: a color like "red", this will be converted into a
 * valid ansi color code if the color is valid
 */
void tm_tag::handle_add(const std::string &tag_name, std::string color){
    if (tag_name.length() > MAX_TAG_LENGTH) {
        std::cerr << "ERROR: Name of tag, '" << tag_name 
                  << "' exceeds the maximum length of " << MAX_TAG_LENGTH 
                  << " characters." << std::endl;
        exit(1);
    }
    // check to make sure that the color is valid,
    auto it = tm_color::VALID_COLORS.find(color);
    if (it == tm_color::VALID_COLORS.end()) {
        std::cerr << "ERROR: '" << color
                  << "' is not a valid color" << std::endl;
        exit(1);
    }
    auto db = tm_db::TMDatabase();
    tm_db::Tag tag = {tag_name, color};
    db.insert_tag(tag);
}


/**
 * displays a list of the tags from the database
 * @param[in] tag_name: a string of the tag specific task
 * @param[in] color: a color like "red", this will be converted into a
 * valid ansi color code if the color is valid
 */
void tm_tag::handle_list(bool no_color, int max_tags){
    auto db = tm_db::TMDatabase();
    db.list_tags(no_color, max_tags);
}
