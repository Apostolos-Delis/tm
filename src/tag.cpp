//
// All the defined constants and functions relevant to tags, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//


#include <string>

#include "tag.hpp"


// Remove a tag from the database, if hard is specified, the tag
// will get removed even if there is tasks that use it
void tm_tag::handle_rm(const std::string &tag_name, bool hard){

}


// Add a new tag to the database, will do nothing if the tag
// already exists
void tm_tag::handle_add(const std::string &tag_name, std::string color){

}


// display the list of tags, no_color minimizes the amount of information
// shown, max tags defines the maximum number of tags to print, if
// max tags is 0, then the amount of tags printed will be all of them
void tm_tag::handle_list(bool no_color, int max_tags){

}
