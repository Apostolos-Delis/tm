//
// Definitions of everything regarding the internal database that tm manages
//

#include <stdlib.h>
#include <sqlite3.h>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

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
                      << sqlite3_errmsg(this->db_) << std::endl;
            exit(-1);
        }
    }
}


// Closes the database
tm_db::TMDatabase::~TMDatabase() {
    sqlite3_close(this->db_);
}


/**
 * Executes a sql query
 * @param[in] query: a string containing the query to be run, must be
 *      sqlite3 SQL
 * @param[in] callback: static int function, that processes one row
 *     returned from the query, look at sqlite3 documentation for more
 *     information on sqlite3_callback
 * @param[in] err_message: the message to display if there is an error
 *     with the SQL query
 */
void tm_db::TMDatabase::execute_query(const std::string &query,
                                      const sqlite3_callback callback,
                                      const std::string &err_message) {
    char* err = NULL;
    int rc = sqlite3_exec(this->db_, query.c_str(), callback, 0, &err);
    if(rc != SQLITE_OK){
        std::cerr << err_message << ": " << err << std::endl;
        sqlite3_free(err);
    }
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
    const std::string sql = "CREATE TABLE IF NOT EXISTS tags (\n"
            "\tid       INTEGER PRIMARY KEY NOT NULL,\n"
            "\tname     VARCHAR(64) UNIQUE,\n"
            "\tcolor    VARCHAR(32)\n"
            ");";
    std::string err_message = "SQL error creating tags table";
    this->execute_query(sql, NULL, err_message);
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


/**
 * Inserts a tag into the tags table
 * @param[in] tag: the tag to be inserted
 */
void tm_db::TMDatabase::insert_tag(const Tag &tag) {
    this->create_tag_table();

    // check to make sure that the color is valid,
    auto it = tm_color::VALID_COLORS.find(tag.color);
    if (it == tm_color::VALID_COLORS.end()) {
        std::cerr << "ERROR: " << tag.color
                  << " is not a valid color" << std::endl;
        exit(-1);
    }

    // Format the string so that name and color are inserted, but
    // id is autoincremented, by sqlite
    std::stringstream ss;
    ss << "INSERT INTO tags (name, color)\nVALUES ('"
       << tag.name << "', '" << tag.color << "');";
    std::string sql(ss.str());

    this->execute_query(sql, NULL, "SQL error Inserting tag into table");
}


/**
 * Inserts a tag into the tags table
 * @param[in] tag: the name of the tag to be removed
 * @param[in] hard: if true, will hard remove the tag, this can be
 * specified with tm tag rm --hard
 */
void tm_db::TMDatabase::remove_tag(const std::string &tag, bool hard) {
    this->create_tag_table();
    this->create_task_tag_table();

    if (hard) {
        std::stringstream ss;
        // This works because the name of the tag must be unique
        // TODO (21/06/2019): Replace this with tag_id
        ss << "DELETE FROM tags WHERE name = '" << tag << "';";
        std::string sql(ss.str());
        this->execute_query(sql, NULL, "SQL error remove tag from table");
    } else {
        // TODO (21/06/2019): Implement the non-hard removal
    }
}

/**
 * Description: callback functions for list_tags, look at sqlite3 documentation
 * for more information about the different params
 */
static int _color_list_tags(void* data, int argc, char** argv, char** cols) {
    for (int i = 0; i < argc; ++i) {
        std::cout <<  argv[i] << ',' << cols[i] << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

/**
 * Same as above function, but this one supports the no_color option
 */
static int _nocolor_list_tags(void* data, int argc, char** argv, char** cols) {
    std::cout << "no_color" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout <<  argv[i] << ',' << cols[i] << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

/**
 * Display the tags
 * @param[in] no_color: will not use color codes to print tags, useful if
 * the output of this is redirected to a file
 * @param[in] max_tags: the maximum number of tags to be displayed
 */
void tm_db::TMDatabase::list_tags(bool no_color, int max_tags) {
    this->create_tag_table();

    std::stringstream ss;
    ss << "SELECT name, color FROM tags";
    if (max_tags > 0) {
        ss << " LIMIT " << max_tags;
    } else if (max_tags < 0) {
        std::cerr << "ERROR: the -m option must recieve a positive value!"
                  << std::endl;
    }
    ss << ";";
    std::string sql(ss.str());

    // Select the correct callback
    sqlite3_callback callback;
    if (no_color) {
        callback = _nocolor_list_tags;
    } else {
        callback = _color_list_tags;
    }
    this->execute_query(sql, callback, "SQL error querying tags");
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

