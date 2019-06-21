//
// Definitions of everything regarding the internal database that tm manages
//

//#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>
#include <vector>
#include <sqlite3.h>

#include "database.hpp"


/**
 * Description: Creates the dotfile directory for tm if it doesn't already
 * exist, then it opens an instance of a sqlite3 database for storing all the
 * data from tasks, sessions, and tags
 */
tm_db::TMDatabase::TMDatabase() {
    // Creates the directory for the database
    std::string homedir = tm_utils::home_dir();
    std::string tm_dir = homedir + "/.tm.d";
    tm_utils::mkdir(tm_dir.c_str());

    std::string db_file = tm_dir + "/data.db";
    int exit_code = sqlite3_open(db_file.c_str(), &db_);

    if (exit_code) {
        system("mkdir ~/.tm.d/");
        exit_code = sqlite3_open(db_file.c_str(), &db_);
        if (exit_code) {
            std::cerr << "Error opening database "
                      << sqlite3_errmsg(db_) << std::endl;
            exit(-1);
        }
    }
}


// Closes the database
tm_db::TMDatabase::~TMDatabase() {
    sqlite3_close(db_);
}


/**
 * Description: gets the id of a tag in the table
 * @param[in] tag: is the tag name in question, if the tag name is invalid,
 * this function will raise an error and exit
 * @return returns an int of the id of the tag found in the tags table
 */
int tm_db::TMDatabase::tag_id(const std::string &tag) {
    return 0;
}


/**
 * Description: gets the id of a task in the table
 * @param[in] task_name: is the task name in question, if the tag name is invalid,
 * this function will raise an error and exit
 * @return returns an int of the id of the tag found in the tags table
 */
int tm_db::TMDatabase::task_id(const std::string &task_name) {
    return 0;
}


/**
 * Creates the tags table in the database if the tags table doesn't already exist
 * The columns for the table are:
 *  id: this is an autoincremented int
 *  name: a string for the task name
 *  color: a string containing the actual text of the color, like "red",
 *         or "light-blue"
 */
void tm_db::TMDatabase::create_tag_table() {
    const char* sql = "CREATE TABLE IF NOT EXISTS tags (\n"
            "\tid       INTEGER PRIMARY KEY NOT NULL,\n"
            "\tname     VARCHAR(64),\n"
            "\tcolor    VARCHAR(32)\n"
            ");";
    char* err_message = NULL;

    int rc = sqlite3_exec(db_, sql, NULL, 0, &err_message);
    if(rc != SQLITE_OK){
        std::cerr << "SQL error creating tags table: "
                  << err_message << std::endl;
        sqlite3_free(err_message);
    }
}


/**
 * Creates the task table in the database if the task table doesn't already exist
 */
void tm_db::TMDatabase::create_task_table() {

}

/**
 * Creates the task table in the database if the tags table doesn't already exist
 */
void tm_db::TMDatabase::create_sess_table() {

}


void tm_db::TMDatabase::create_task_tag_table() {

}


void tm_db::TMDatabase::insert_tag(const Tag &tag) {
    this->create_tag_table();

}


void tm_db::TMDatabase::remove_tag(const std::string &tag) {

}


void tm_db::TMDatabase::list_tags(bool no_color, int max_tags) {

}


void tm_db::TMDatabase::complete_task(const std::string &task_name) {

}


void tm_db::TMDatabase::remove_task(const std::string &task) {

}


void tm_db::TMDatabase::add_task(const Task &task) {

}


void list_tasks(bool condensed, int max_tags, bool display_done,
                const std::vector<std::string> &specified_tags,
                const std::string &specified_date) {

}


void tm_db::TMDatabase::sess_log(bool condensed, int max_sessions) {

}


void tm_db::TMDatabase::add_sess(const std::string &start,
                                 const std::string &stop,
                                 const std::string &task,
                                 const std::string &description) {
}

