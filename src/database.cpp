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
            exit(1);
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
 * Description: Callback for num rows
 * Reference:
 * https://stackoverflow.com/questions/21692245/retrieve-sql-table-row-count-with-c
 */
static int count_callback(void *count, int argc, char **argv, char **azColName) {
    int* c = (int*) count;
    *c = atoi(argv[0]);
    return 0;
}

/**
 * Description: returns the number of entries in a table in the database
 * @param[in] table: the name of the table to inspect
 * @return an int of the number of rows in a table, returns -1 if no table
 * is identified
 */
int tm_db::TMDatabase::num_rows(const std::string &table){
    int num_rows = -1;

    std::stringstream ss;
    ss << "SELECT COUNT(*) FROM " << table;
    std::string sql(ss.str());

    char* err_message;
    int rc = sqlite3_exec(this->db_, sql.c_str(),
                          count_callback, &num_rows, &err_message);
    if(rc != SQLITE_OK){
        std::cerr << "ERROR Reading the number of rows from table '"
                  << table << "': " << err_message<< std::endl;
        sqlite3_free(err_message);
    }
    return num_rows;
}


/**
 * Description: gets the id of a tag in the table
 * @param[in] tag: is the tag name in question, if the tag name is invalid,
 * this function will raise an error and exit
 * @return returns an int of the id of the tag found in the tags table
 */
int tm_db::TMDatabase::tag_id(const std::string &tag) {
    sqlite3_stmt* sql;

    sqlite3_prepare_v2(this->db_, "SELECT id FROM tags WHERE name = ?1", -1, &sql, NULL);

    sqlite3_bind_text(sql, 1, tag.c_str(), -1, SQLITE_STATIC);
    int tag_id = -1;

    int rc;
    // On paper, this should only loop once since tag names should be unique
    while ( (rc = sqlite3_step(sql)) == SQLITE_ROW) {                                              /* 2 */
        tag_id = sqlite3_column_int(sql, 0);
    }
    return tag_id;
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
    // TODO (09/07/2019): Add foreign key constraint for task_id
    const std::string sql = "CREATE TABLE IF NOT EXISTS task_tags (\n"
            "\ttag_id       INTEGER NOT NULL,\n"
            "\ttask_id       INTEGER NOT NULL,\n"
            "PRIMARY KEY (tag_id, task_id)\n"
            "FOREIGN KEY (tag_id) REFERENCES tags(id)"
            ");";
    std::string err_message = "SQL error creating task_tags table";
    this->execute_query(sql, NULL, err_message);
}


/**
 * Inserts a tag into the tags table, if tag already exists, the color is
 * updated to reflect the new color selected
 * @param[in] tag: the tag to be inserted
 */
void tm_db::TMDatabase::insert_tag(const Tag &tag) {
    this->create_tag_table();

    // check to make sure that the color is valid,
    auto it = tm_color::VALID_COLORS.find(tag.color);
    if (it == tm_color::VALID_COLORS.end()) {
        std::cerr << "ERROR: " << tag.color
                  << " is not a valid color" << std::endl;
        exit(1);
    }

    // Format the string so that name and color are inserted, but
    // id is autoincremented, by sqlite
    std::stringstream ss;
    ss << "INSERT INTO tags (name, color)\nVALUES ('"
       << tag.name << "', '" << tag.color << "') "
       << "ON CONFLICT(name) DO UPDATE SET color = '"
       << tag.color << "';";
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

    int tag_id = this->tag_id(tag);

    // Returns the total number of times that the tag is used by creating a
    // subquery and treating it as a table
    std::stringstream ref_ss;
    ref_ss << "(SELECT * FROM task_tags WHERE task_id = " << tag_id << ")";
    std::string referenced_sql(ref_ss.str());
    int num_referenced = this->num_rows(referenced_sql);

    if (hard || num_referenced == 0) {
        // This removal method does not check to see if other tags exist
        std::stringstream ss1, ss2;
        ss1 << "DELETE FROM tags WHERE id = " << tag_id << ";";
        this->execute_query(ss1.str(), NULL, "SQL error remove tag from table");
        if (num_referenced != 0) {
            ss2 << "DELETE FROM task_tags WHERE tag_id = " << tag_id << ";";
            this->execute_query(ss2.str(), NULL, "SQL error remove tag from table");
        }
    } else {
        std::cerr << "ERROR: Cannot remove tag '" << tag 
                  << "' because it is currently referenced by "
                  << num_referenced << " tasks."<< std::endl;
        exit(1);
    }
}


#define SPACE_WIDTH 20

/**
 * Description: callback functions for list_tags, look at sqlite3 documentation
 * for more information about the different params
 */
static int _color_list_tags(void* data, int argc, char** argv, char** cols) {
    size_t tag_len = strlen(argv[0]);
    std::string spaces((SPACE_WIDTH - tag_len), ' ');

    std::string color(argv[1]);
    std::string code = tm_color::COLOR_CODES.find(color)->second;

    std::cout << argv[0] <<  spaces << code << argv[1]
              << tm_color::NOCOLOR << std::endl;
    return 0;
}


/**
 * Same as above function, but this one supports the no_color option
 */
static int _nocolor_list_tags(void* data, int argc, char** argv, char** cols) {
    size_t tag_len = strlen(argv[0]);
    std::string spaces((SPACE_WIDTH - tag_len), ' ');
    std::cout << argv[0] <<  spaces << argv[1] << std::endl;
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

    // Print the top of the table
    if (this->num_rows("tags") > 0) {
        std::string spaces((SPACE_WIDTH - 3), ' ');
        std::cout << "\033[4;49;39mTag" << spaces << "Color\033[0m" << std::endl;
    }

    // Select the correct callback
    sqlite3_callback callback;
    if (no_color) {
        callback = _nocolor_list_tags;
    } else {
        callback = _color_list_tags;
    }
    this->execute_query(sql, callback, "SQL error querying tags");
}


/**
 * Description: Update a task to complete in the tasks table
 * @param[in] task_name: The name of the task to update to complete
 */
void tm_db::TMDatabase::complete_task(const std::string &task_name) {

}


/**
 * Description: remove a task from the tasks table
 * @param[in] task_name: The name of the task to remove
 */
void tm_db::TMDatabase::remove_task(const std::string &task) {

}


/**
 * Description: adds a new task to the tasks table
 * @param[in] task: the task to be inserted
 */
void tm_db::TMDatabase::add_task(const Task &task) {

}


/**
 * Description: display a list of tasks, matching the criteria
 * @param[in] condensed: display all the tasks with a minimized output
 * @param[in] max_tasks: the maximum number of tasks to display
 * @param[in] display_done: if true will also display completed tags that
 * match the specified criteria
 * @param[in] specified tags: only display tags that have one of the tags
 * @param[in] specified date: only display tags that are due on the
 * specified date
 */
void tm_db::TMDatabase::list_tasks(bool condensed, int max_tags, bool display_done,
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
