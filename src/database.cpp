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

#define TM_DIR "/.tm.d"
#define DB_FILE "/data.sqlite3"


/**
 * Description: Creates the dotfile directory for tm if it doesn't already
 * exist, then it opens an instance of a sqlite3 database for storing all the
 * data from tasks, sessions, and tags
 */
tm_db::TMDatabase::TMDatabase() {
    // Creates the directory for the database
    std::string homedir = tm_utils::home_dir();
    std::string tm_dir = homedir + TM_DIR;
    tm_utils::mkdir(tm_dir.c_str());

    std::string db_file = tm_dir + DB_FILE;
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
 * @param[in] task_name: is the task name in question, if the task name is invalid,
 * this function will raise an error and exit
 * @return returns an int of the id of the tag found in the task table
 */
int tm_db::TMDatabase::task_id(const std::string &task_name) {
    sqlite3_stmt* sql;
    sqlite3_prepare_v2(this->db_, "SELECT id FROM tasks WHERE task = ?1", -1, &sql, NULL);
    sqlite3_bind_text(sql, 1, task_name.c_str(), -1, SQLITE_STATIC);

    int task_id = -1;
    int rc;
    // On paper, this should only loop once since tag names should be unique
    while ( (rc = sqlite3_step(sql)) == SQLITE_ROW) {                                              /* 2 */
        task_id = sqlite3_column_int(sql, 0);
    }
    return task_id;
}


/**
 * Description: gets the id of a proj in the table
 * @param[in] proj_name: is the proj name in question, if the proj name
 * is invalid, this function will raise an error and exit
 * @return returns an int of the id of the proj found in the tags table
 */
int tm_db::TMDatabase::proj_id(const std::string &proj_name) {
    sqlite3_stmt* sql;
    sqlite3_prepare_v2(this->db_, "SELECT id FROM projects WHERE name = ?1", -1, &sql, NULL);
    sqlite3_bind_text(sql, 1, proj_name.c_str(), -1, SQLITE_STATIC);

    int proj_id = -1;
    int rc;
    // On paper, this should only loop once since tag names should be unique
    while ( (rc = sqlite3_step(sql)) == SQLITE_ROW) {                                              /* 2 */
        proj_id = sqlite3_column_int(sql, 0);
    }
    return proj_id;
}


/**
 * Creates the tags table in the database if the tags table doesn't already exist
 * The columns for the table are:
 *  id: this is an autoincremented int
 *  name: a string for the tag name
 *  color: a string containing the actual text of the color, like "red",
 *         or "light-blue"
 */
void tm_db::TMDatabase::create_tag_table() {
    const std::string sql = "CREATE TABLE IF NOT EXISTS tags (\n"
            "\tid       INTEGER PRIMARY KEY NOT NULL,\n"
            "\tname     VARCHAR(20) UNIQUE NOT NULL,\n"
            "\tcolor    VARCHAR(20) NOT NULL\n"
            ");";
    std::string err_message = "SQL error creating tags table";
    this->execute_query(sql, NULL, err_message);
}


/**
 * Creates the tasks table in the database if the tasks table doesn't
 * already exist
 * The columns for the table are:
 *   id: this is an autoincremented int
 *   task: a string for the task to complete
 *   due: a string containing the date as a string in the following format:
 *   YYYY-MM-DD HH:MM:SS.SSS
 *   complete: boolean for whether a certain task is complete
 */
void tm_db::TMDatabase::create_task_table() {
    const std::string sql = "CREATE TABLE IF NOT EXISTS tasks (\n"
            "\tid       INTEGER PRIMARY KEY NOT NULL,\n"
            "\ttask     VARCHAR(128) UNIQUE,\n"
            "\tproj_id  INTEGER DEFAULT NULL,\n"
            "\tcomplete INTEGER DEFAULT 0 NOT NULL,\n"
            "\tdue      TEXT,\n"
            "FOREIGN KEY (proj_id) REFERENCES projects(id)\n"
            ");";
    std::string err_message = "SQL error creating tasks table";
    this->execute_query(sql, NULL, err_message);
}

/**
 * Creates the task table in the database if the tags table doesn't already exist
 */
void tm_db::TMDatabase::create_sess_table() {
    // TODO (25/07/2019): Create Sess table

}


/**
 * Creates a table for projects
 * The columns are:
 *   id: an autincrementing int representing a unique id for projects
 *   name: the name of the project
 *   complete: bool representing if the project is complete
 */
void tm_db::TMDatabase::create_proj_table() {
    const std::string sql = "CREATE TABLE IF NOT EXISTS projects (\n"
            "\tid       INTEGER PRIMARY KEY NOT NULL,\n"
            "\tname     VARCHAR(64) UNIQUE,\n"
            "\tcomplete INTEGER DEFAULT 0 NOT NULL\n"
            ");";
    std::string err_message = "SQL error creating proj table";
    this->execute_query(sql, NULL, err_message);
}


void tm_db::TMDatabase::create_task_tag_table() {
    const std::string sql = "CREATE TABLE IF NOT EXISTS task_tags (\n"
            "\ttag_id       INTEGER NOT NULL,\n"
            "\ttask_id       INTEGER NOT NULL,\n"
            "PRIMARY KEY (tag_id, task_id)\n"
            "FOREIGN KEY (tag_id) REFERENCES tags(id)\n"
            "FOREIGN KEY (task_id) REFERENCES tasks(id)"
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

    // Format the string so that name and color are inserted, but
    // id is autoincremented, by sqlite
    std::stringstream ss;
    ss << "INSERT INTO tags (name, color)\nVALUES ('"
       << tag.name << "', '" << tag.color << "') "
       << "ON CONFLICT(name) DO UPDATE SET color = '"
       << tag.color << "';";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error inserting tag into table");
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
void tm_db::TMDatabase::complete_task(int task_id) {
    this->create_task_table();
    std::stringstream ss;
    ss << "UPDATE tasks\nSET complete = 1\nWHERE id = "
       << task_id << ";";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error updating task table");
}


/**
 * Description: remove a task from the tasks table
 * @param[in] task_id: The id of the task to remove
 */
void tm_db::TMDatabase::remove_task(int task_id) {
    // TODO (11/07/2019): Add a --hard option for if the task is used in a session
    this->create_task_table();
    std::stringstream ss;
    ss << "DELETE FROM tasks WHERE id = " << task_id << ";";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error removing task from task table");
}


/**
 * Description: adds a new task to the tasks table
 * @param[in] task: the task to be inserted
 */
void tm_db::TMDatabase::add_task(const Task &task) {
    this->create_task_table();
    std::stringstream ss;
    if (!task.proj_name.empty()) {

        int proj_id = this->proj_id(task.proj_name);
        if (proj_id == -1) {
            std::cerr << "ERROR: the project specified for task: '"
                      << task.name << "' is invalid" << std::endl;
            std::cerr << "Invalid project: " << task.proj_name << std::endl;
            exit(1);
        }
        ss << "INSERT INTO tasks (task, due, proj_id)\nVALUES('"
           << task.name << "', '" << task.due << "', "
           << proj_id << ") ";
    } else {
        ss << "INSERT INTO tasks (task, due)\nVALUES('"
           << task.name << "', '" << task.due << "') ";
    }
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error inserting task into table");
}


/**
 * Description: Callback function for tags, unfortunantly, because the method
 */
int static list_tasks_callback(void* data, int argc,
                               char** argv, char** cols) {
    std::string spaces1(7 - strlen(argv[0]), ' ');
    std::string spaces2(6, ' ');
    std::string spaces3(5, ' ');

    std::string completed;
    if (atoi(argv[1])) {
        completed = "\033[32m✔\033[0m";
    } else {
        completed = "\033[31m✖\033[0m";
    }

    std::string date((const char*) argv[2]);
    date = date.substr(0, 16);

    std::cout << argv[0] << spaces1 << completed << spaces2 << date << spaces3 
              << argv[3] << std::endl;
    return 0;
}


/**
 * Description: Callback function for list_tags, unfortunantly, because the
 * method has to be static, a new pointer must be created to the directory to
 * query each of the tags for each of the tasks, because of this, this is one
 * of the most costly operations of tm
 */
int static list_tasks_callback_long(void* data, int argc,
                                    char** argv, char** cols) {
    std::string homedir = tm_utils::home_dir();
    std::string db_file = homedir + TM_DIR + DB_FILE;
    sqlite3 *db;
    int exit_code = sqlite3_open(db_file.c_str(), &db);
    if (exit_code) {
        std::cerr << "Error opening database "
                  << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    std::string spaces1(7 - strlen(argv[0]), ' ');
    std::string spaces2(7 - strlen(argv[0]), ' ');
    std::string spaces3(7 - strlen(argv[0]), ' ');
    std::string spaces4(7 - strlen(argv[0]), ' ');

    size_t tag_len = strlen(argv[0]);
    std::string spaces((SPACE_WIDTH - tag_len), ' ');

    //std::string color(argv[1]);
    //std::string code = tm_color::COLOR_CODES.find(color)->second;

    //std::cout << argv[0] <<  spaces << code << argv[1]
              //<< tm_color::NOCOLOR << std::endl;
    sqlite3_close(db);
    return 0;
}

/**
 * Description: display a list of tasks, matching the criteria
 * @param[in] list_long: display additional information about each task
 * @param[in] max_tasks: the maximum number of tasks to display
 * @param[in] display_done: if true will also display completed tags that
 * match the specified criteria
 * @param[in] specified tags: only display tags that have one of the tags
 * @param[in] specified date: only display tags that are due on the
 * specified date
 */
void tm_db::TMDatabase::list_tasks(bool list_long, int max_tasks,
                                   bool display_done,
                                   const std::vector<std::string> &specified_tags,
                                   const std::string &specified_date) {
    // TODO (25/07/2019): Finish
    this->create_task_table();

    int num_tasks;
    if (display_done) {
        num_tasks = this->num_rows("tasks");
    } else {
        num_tasks = this->num_rows("(SELECT id FROM tasks WHERE complete = 0)");
    }
    if (num_tasks == 0) { return; }

    std::stringstream ss;
    if (list_long) {
        ss << "SELECT * FROM tags";
    } else {
        ss << "SELECT id, complete, due, task FROM tasks\n";
        std::string space1(4, ' ');
        std::string space2(8, ' ');
        std::cout << "\033[4;49;39mID    Done" << space1 
                  << "Due Date/Time" << space2 << "Task\033[0m" << std::endl;
    }

    if (!display_done) {
        ss << "WHERE complete = 0\n";
    }

    if (max_tasks > 0 && num_tasks > max_tasks) {
        ss << " LIMIT " << max_tasks;
    } else if (max_tasks < 0) {
        std::cerr << "ERROR: the -m option must recieve a positive value!"
                  << std::endl;
    }
    ss << ";";
    std::string sql(ss.str());

    // Print the top of the table

    sqlite3_callback callback;
    if (list_long) {
        callback = list_tasks_callback_long;
    } else {
        callback = list_tasks_callback;
    }
    this->execute_query(sql, callback, "SQL error querying tasks");
}


void tm_db::TMDatabase::sess_log(bool condensed, int max_sessions) {
    // TODO (25/07/2019): Finish
}


void tm_db::TMDatabase::add_sess(const std::string &start,
                                 const std::string &stop,
                                 const std::string &task,
                                 const std::string &description) {
        // TODO (25/07/2019): Finish
}


/**
 * Description: remove a project from the projects table
 * @param[in] proj_name: The name of the project to remove
 * @param[in] hard: If true, then the project gets removed from the database, and
 * all the tasks that belong to the project have their project set to null
 * @return Returns
 */
void tm_db::TMDatabase::remove_project(std::string proj_name, bool hard) {
    this->create_proj_table();
    int proj_id = this->proj_id(proj_name);

    if (hard) {
        // Set all proj_id from tasks to NULL since the project is getting deleted
        std::stringstream ss;
        ss << "UPDATE tasks\nSET proj_id = NULL\nWHERE id = " << proj_id;
        this->execute_query(ss.str(), NULL,
                "SQL error updating proj_ids for tasks table");
    }
    std::stringstream ss;
    ss << "DELETE FROM projects WHERE id = " << proj_id << ";";
    this->execute_query(ss.str(), NULL, "SQL error removing project from table");
}


/**
 * Description: Set the projects status to complete, if there are still tasks that
 * reference that project and are unfinished, then this will fail
 * @param[in] proj_name: The name of the project to complete
 */
void tm_db::TMDatabase::complete_project(std::string proj_name) {
    this->create_proj_table();
    int proj_id = this->proj_id(proj_name);
    std::stringstream ss;
    ss << "UPDATE projects\nSET complete = 1,\nWHERE id = "
       << proj_id << ";";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error updating projects table");

}


/**
 * Description: adds a new project to the project table
 * @param[in] proj_name: the name of the project to be added
 */
void tm_db::TMDatabase::add_project(const std::string &proj_name) {
    this->create_proj_table();

    std::stringstream ss;
    ss << "INSERT INTO projects (name)\nVALUES ('" << proj_name << "');";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error Inserting proj into table");
}


/**
 * Description: display a list of projects, matching the criteria
 * @param[in] show_tasks: display all the projects with their corresponding tasks
 * @param[in] display_done: if true will also display completed projects that
 * match the specified criteria, if the long option is true, displays completed
 * tasks as well from those projects
 * @param[in] proj_ids: only display tasks from those specific projects, this
 * will automatically set the long option to true
 */
void tm_db::TMDatabase::list_projects(bool show_tasks, bool display_done,
                                      const std::vector<std::string> &proj_ids) {
    this->create_proj_table();
    // TODO (25/07/2019): Finish
}
