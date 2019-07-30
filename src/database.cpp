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
        exit(1);
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
        exit(1);
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
    while ((rc = sqlite3_step(sql)) == SQLITE_ROW) {
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
            "\ttask     VARCHAR(128),\n"
            "\tproj_id  INTEGER DEFAULT NULL,\n"
            "\tcomplete INTEGER DEFAULT 0 NOT NULL,\n"
            "\tdue      VARCHAR(24),\n"
            "FOREIGN KEY (proj_id) REFERENCES projects(id)\n"
            ");";
    std::string err_message = "SQL error creating tasks table";
    this->execute_query(sql, NULL, err_message);
}

/**
 * Creates the sess table in the database
 * Supports the following columns:
 *  id: a primary key integer
 *  task_id: a reference to the task being worked on
 *  time_started: the time a task was started: in the form:
 *   YYYY-MM-DD HH:MM:SS.SSS
 *  length: integer representing the number of seconds the sess lasted
 *  desc: a small description of the session
 */
void tm_db::TMDatabase::create_sess_table() {
    const std::string sql = "CREATE TABLE IF NOT EXISTS sess (\n"
            "\tid                INTEGER PRIMARY KEY NOT NULL,\n"
            "\ttask_id           VARCHAR(128),\n"
            "\ttime_started      TEXT,\n"
            "\tlength            INTEGER NOT NULL,\n"
            "\tdesc              TEXT DEFAULT NULL,\n"
            "FOREIGN KEY (task_id) REFERENCES tasks(id)\n"
            ");";
    std::string err_message = "SQL error creating sess table";
    this->execute_query(sql, NULL, err_message);
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
static int color_list_tags(void* data, int argc, char** argv, char** cols) {
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
static int nocolor_list_tags(void* data, int argc, char** argv, char** cols) {
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
        std::cout << "\033[1;4;49;39mTag" << spaces << "Color\033[0m" << std::endl;
    }

    // Select the correct callback
    sqlite3_callback callback;
    if (no_color) {
        callback = nocolor_list_tags;
    } else {
        callback = color_list_tags;
    }
    this->execute_query(sql, callback, "SQL error querying tags");
}


/**
 * Description: Check to see if the task_id corresponds to a valid incomplete
 * task in the tasks table
 * @param[in] task_id: the task id to be checkede
 * @return Returns true if the task_id is valid and the task isn't completed
 */
bool tm_db::TMDatabase::valid_task_id(int task_id) {
    std::stringstream ss;
    ss << "(SELECT id FROM tasks WHERE complete = 0 AND id = " << task_id << ")";
    return this->num_rows(ss.str()) == 1;
}


/**
 * Description: Update a task to complete in the tasks table
 * @param[in] task_name: The name of the task to update to complete
 * @param[in] val: The value for complete to be set to, 0 if it should
 * be set to incomplete, 1 if complete
 *
 */
void tm_db::TMDatabase::complete_task(int task_id, int val) {
    this->create_task_table();
    std::stringstream ss;
    ss << "UPDATE tasks\nSET complete = " << val << "\nWHERE id = "
       << task_id << ";";
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error updating task table");
}


/**
 * Description: remove a task from the tasks table
 * @param[in] task_id: The id of the task to remove
 */
void tm_db::TMDatabase::remove_task(int task_id) {
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
    this->create_task_tag_table();
    std::stringstream ss;
    if (!task.proj_name.empty()) {

        int proj_id = this->proj_id(task.proj_name);
        if (proj_id == -1) {
            std::cerr << "ERROR: the project specified for task: '"
                      << task.name << "' is invalid" << std::endl;
            std::cerr << "Invalid project: " << task.proj_name << std::endl;
            exit(1);
        }
        // Check to make sure that the proj_id is not for a completed project
        std::stringstream ss_check;
        ss_check << "(SELECT 1 FROM projects WHERE complete = 0 AND id = " << proj_id << ")";
        if (this->num_rows(ss_check.str()) == 0) {
            std::cerr << "ERROR: '" << task.proj_name << "' is competed!" << std::endl;
            std::cerr << "Run 'tm proj done -r -n " << task.proj_name
                      << "' to set the project to be still in progress" << std::endl;
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

    int task_id = this->task_id(task.name);
    int tag_id;
    for (auto tag: task.tags) {
        tag_id = this->tag_id(tag);
        if (tag_id == -1) {
            std::cerr << "ERROR: '" << tag
                      << "' is not a valid tag." << std::endl;
            exit(1);
        }
        std::stringstream ss1;
        ss1 << "INSERT INTO task_tags (task_id, tag_id) VALUES\n"
            << "("<< task_id << ", " << tag_id << ");";
        this->execute_query(ss1.str(), NULL,
                            "SQL error inserting into task_tag table");
    }
}


/**
 * Description: Callback function for tm task list
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

    std::cout << argv[0] << spaces1 << completed << spaces2;
    if (date < tm_utils::current_datetime()) {
        std::cout << "\033[91m" << date << tm_color::NOCOLOR << spaces3;
    } else {
        std::cout << date << spaces3;
    }
    std::cout << argv[3] << std::endl;
    return 0;
}


/**
 * Description: Callback to display the tags for each task when
 * tm task list --long is called
 */
int static task_tag_callback(void* data, int argc,
                             char** argv, char** cols) {
    std::string color(argv[1]);
    std::string code = tm_color::COLOR_CODES.find(color)->second;

    std::cout << code << argv[0] << "\033[0m, " << std::flush;
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
    list_tasks_callback(data, argc, argv, cols);
    if (argv[4]) {
        std::cout << "\033[1;39mProject: \033[0m" << argv[4] << std::endl;
    }

    /**
     * Here is the query in a more legible fashion, essentially
     * get all the tag components relating to this specific task
     *
     * SELECT
     *      tags.name,
     *      tags.color,
     * FROM tags
     * INNER JOIN task_tags ON task_tags.tag_id = tags.id
     * INNER JOIN tasks ON task_tags.task_id = tasks.id
     * WHERE tasks.name = 'argv[3]'
     */
    std::stringstream ss1;
    ss1 << "SELECT name, color\nFROM tags\n"
        << "INNER JOIN task_tags ON task_tags.tag_id = tags.id\n"
        << "INNER JOIN tasks ON task_tags.task_id = tasks.id\n"
        << "WHERE tasks.task = '" << argv[3] << "'";

    int num_rows = -1;

    std::stringstream ss;
    ss << "SELECT COUNT(*) FROM (" << ss1.str() << ")";

    char* err_message;
    int rc = sqlite3_exec(db, ss.str().c_str(),
                          count_callback, &num_rows, &err_message);
    if(rc != SQLITE_OK){
        std::cerr << "ERROR getting the number of tags related to task'"
                  "\nSQL ERROR: " << err_message << std::endl;
        sqlite3_free(err_message);
        exit(1);
    }

    if (num_rows > 0) {
        std::cout << "\033[1;39mTags: \033[0m";
        char* err = NULL;
        rc = sqlite3_exec(db, ss1.str().c_str(),
                              task_tag_callback, 0, &err);
        if(rc != SQLITE_OK){
            std::cerr << "Error getting tags: " << err << std::endl;
            sqlite3_free(err);
            exit(1);
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
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
 * @param[in] specified_proj: only display tags that are due in the
 * specified project
 */
void tm_db::TMDatabase::list_tasks(bool list_long, int max_tasks,
                                   bool display_done,
                                   const std::vector<std::string> &specified_tags,
                                   const std::string &specified_date,
                                   const std::string &specified_proj) {
    this->create_task_table();

    std::stringstream ss;
    if (list_long) {
        ss << "SELECT DISTINCT tasks.id, tasks.complete, tasks.due, tasks.task, "
           << "projects.name FROM tasks"
           << "\nLEFT JOIN projects ON tasks.proj_id = projects.id\n";
    } else {
        ss << "SELECT DISTINCT tasks.id, tasks.complete, tasks.due, "
           << "tasks.task FROM tasks\n";
    }
    if (!specified_tags.empty()) {
        ss << "LEFT JOIN task_tags ON tasks.id = task_tags.task_id\n";
    }
    ss << "WHERE 1 = 1\n";
    if (!display_done) {
        ss << "AND tasks.complete = 0\n";
    }
    if (!specified_date.empty()) {
        ss << "AND date(tasks.due) = date('" << specified_date << "')\n";
    }
    if (!specified_proj.empty()) {
        int proj_id = this->proj_id(specified_proj);
        if (proj_id == -1) {
            std::cerr << "ERROR: '" << specified_proj
                      << "' is not an existing project!" << std::endl;
            exit(1);
        }
        ss << "AND tasks.proj_id = " << proj_id << "\n";
    }
    if (!specified_tags.empty()) {
        ss << "AND (";
        for (int i = 0; i < specified_tags.size(); ++i) {
            auto tag = specified_tags[i];
            int tag_id = this->tag_id(tag);
            if (tag_id == -1) {
                std::cerr << "ERROR: '" << tag << "' is invalid" << std::endl;
                exit(1);
            }
            if (i != 0) {
                ss << " OR";
            }
            ss << " task_tags.tag_id = " << tag_id;
        }
        ss << ")\n";
        ss << "GROUP BY task_id\n";
    }
    if (display_done) {
        ss << "ORDER BY due DESC\n";
    } else {
        ss << "ORDER BY due ASC\n";
    }


    if (max_tasks > 0) {
        ss << " LIMIT " << max_tasks;
    } else if (max_tasks < 0) {
        std::cerr << "ERROR: the -m option must recieve a positive value!"
                  << std::endl;
        exit(1);
    }
    std::string sql(ss.str());

    int num_tasks = this->num_rows("(" + sql + ")");
    if (num_tasks == 0) {
        return;
    }

    // Print the top of the table
    std::string space1(4, ' ');
    std::string space2(6, ' ');
    std::cout << "\033[1;4;49;39mID    Done" << space1
              << "Due Date / Time" << space2 << "Task\033[0m" << std::endl;

    sqlite3_callback callback;
    if (list_long) {
        callback = list_tasks_callback_long;
    } else {
        callback = list_tasks_callback;
    }
    this->execute_query(sql, callback, "SQL error querying tasks");
}


/**
 * Description: Callback function for printing the sessions
 */
static int list_sess_long(void* data, int argc, char** argv, char** cols) {
    std::cout << "Session Number: " << argv[0] << std::endl;
    std::cout << "Task to Complete: '" << argv[1] << "'" << std::endl;
    std::string date_time(argv[2]);
    std::cout << "Time and Date: " << date_time.substr(0, 16) << std::endl;
    std::cout << "Duration: " << tm_utils::sec_to_time(atoi(argv[3]))
              << " (H:MM:SS)" << std::endl;
    std::cout << "\n\t" << argv[4] << std::endl;
    std::cout << std::endl;
    return 0;
}


void tm_db::TMDatabase::sess_log(bool condensed, int max_sessions) {
    this->create_sess_table();
    std::stringstream ss;
    ss << "SELECT sess.id, tasks.task, sess.time_started, sess.length, sess.desc\n"
       << "FROM sess INNER JOIN tasks ON tasks.id = sess.task_id\n"
       << "ORDER BY sess.time_started DESC\n";

    if (max_sessions > 0) {
       ss << "LIMIT " << max_sessions;
    } else if (max_sessions < 0) {
        std::cerr << "ERROR: the -m option must recieve a positive number!"
                  << std::endl;
        exit(1);
    }

    sqlite3_callback callback;
    if (condensed) {
        callback = NULL;
    } else {
        // TODO (30/07/2019): Pipe this to the bash command less
        callback = list_sess_long;
    }
    this->execute_query(ss.str(), callback, "SQL ERROR Querying sessions");
}


/**
 * Description: Inserts a session into the sess table
 * @param[in] start: the start date of the session in proper ISO format
 * @param[in] sess_length: the time in seconds of the sess duration
 * @param[in] task_id: the task_id of the task that was worked on in the sess
 * @param[in] description: a brief description of the sess
 */
void tm_db::TMDatabase::add_sess(const std::string &start,
                                 int sess_length,
                                 int &task_id,
                                 const std::string &description) {
    this->create_sess_table();
    std::stringstream ss;
    if (!description.empty()) {
        ss << "INSERT INTO sess (task_id, time_started, desc, length)\nVALUES("
           << task_id << ", '" << start << "', '" << description << "', "
           << sess_length << ")";
    }

    else {
        ss << "INSERT INTO sess (task_id, time_started, length)\nVALUES("
           << task_id << ", '" << start << "', "
           << sess_length << ")";
    }
    std::string sql(ss.str());
    this->execute_query(sql, NULL, "SQL error inserting sess into table");
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
 * @param[in] val: val will be 1 if project is to be completed, 0 if incomplete
 */
void tm_db::TMDatabase::complete_project(std::string proj_name, int val) {
    this->create_proj_table();
    int proj_id = this->proj_id(proj_name);
    if (proj_id == -1) {
        std::cerr << "ERROR: '" << proj_id << "' is not a valid project"
                  << std::endl;
        exit(1);
    }

    std::stringstream ss_num;
    // Check to see if all the tasks related to this project are complete
    ss_num << "(SELECT * FROM tasks WHERE complete = 0 AND proj_id = "
           << proj_id << ")";

    int num_tasks = this->num_rows(ss_num.str());
    if (num_tasks == 0) {
        std::stringstream ss;
        ss << "UPDATE projects\nSET complete = " << val << "\nWHERE id = "
           << proj_id << ";";
        std::string sql(ss.str());
        this->execute_query(sql, NULL, "SQL error updating projects table");
    } else {
        std::cerr << "ERROR: '" << proj_name
                  << "' cannot be completed because there ";
        if (num_tasks == 1) {
            std::cerr << "is 1 task that is";
        } else {
            std::cerr << "are " << num_tasks << " tasks that are";
        }
        std::cerr << " incomplete for this project." << std::endl;
        std::cerr << "Run 'tm task list -p " << proj_name
                  << "' to see the tasks remaining to be completed."
                  << std::endl;
    }
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
 * Description: Callback function for projects
 */
int static list_projects_callback(void* data, int argc,
                               char** argv, char** cols) {
    std::string completed;
    if (atoi(argv[0])) {
        completed = "\033[32m ✔\033[0m";
    } else {
        completed = "\033[31m ✖\033[0m";
    }
    std::cout << completed << "      " << argv[1] << std::endl;
    return 0;
}

//
// Prints the sample tasks from one project
//
int static proj_task_callback(void* data, int argc,
                              char** argv, char** cols) {
    size_t tag_len = strlen(argv[0]);
    std::string spaces((7 - tag_len), ' ');
    std::string date(argv[1]);
    std::cout << "\t" << argv[0] << spaces << date.substr(0, 10)
              << "  " << argv[2] << std::endl;
    return 0;
}

//
// callback for tm proj list -l
//
int static list_projects_callback_long(void* data, int argc,
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
    list_projects_callback(data, argc, argv, cols);

    std::stringstream ss1;
    ss1 << "SELECT id, due, task\nFROM tasks\n"
        << "WHERE complete = 0 AND proj_id = "
        << argv[2] << "\nORDER BY due DESC\nLIMIT 5";

    int num_rows = -1;
    std::stringstream ss;
    ss << "SELECT COUNT(*) FROM (" << ss1.str() << ")";
    char* err_message;
    int rc = sqlite3_exec(db, ss.str().c_str(),
                          count_callback, &num_rows, &err_message);
    if(rc != SQLITE_OK){
        std::cerr << "ERROR getting the number of tags related to task'"
                  "\nSQL ERROR: " << err_message << std::endl;
        sqlite3_free(err_message);
        exit(1);
    }

    if (num_rows > 0) {
        std::cout << "\033[1;39mTasks:  \033[0m"
                  << "\033[1;4;39mID     Due         Task\033[0m"
                  << std::endl;
        char* err = NULL;
        rc = sqlite3_exec(db, ss1.str().c_str(),
                              proj_task_callback, 0, &err);
        if(rc != SQLITE_OK){
            std::cerr << "Error getting tags: " << err << std::endl;
            sqlite3_free(err);
            exit(1);
        }
    } else {
        std::cout << "\tAll tasks completed at this time." << std::endl;
    }
    std::cout << std::endl;
    sqlite3_close(db);
    return 0;
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
                                      const std::vector<std::string> &projects) {
    this->create_proj_table();

    std::stringstream ss;
    ss << "SELECT complete, name, id FROM projects\n";
    ss << "WHERE 1 = 1\n";
    if (!display_done) {
        ss << "AND complete = 0\n";
    }
    std::string sql(ss.str());

    // Print the top of the table
    if (this->num_rows("(" + sql + ")") > 0) {
        std::cout << "\033[1;4;49;39mDone    Project\033[0m" << std::endl;
    }

    // Select the correct callback
    sqlite3_callback callback;
    if (show_tasks) {
        callback = list_projects_callback_long;
    } else {
        callback = list_projects_callback;
    }
    this->execute_query(sql, callback, "SQL error querying projects");
}
