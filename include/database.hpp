//
// Definitions of everything regarding the internal database that tm manages
// header files, and have this just be an object that effectively wraps around
// the sqlite3 methods
//
//

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <string>
#include <vector>
#include <sqlite3.h>

#include "utils.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


namespace tm_db {

    struct Tag {
        std::string name;
        std::string color;
    };

    struct Task {
        std::string name;
        std::string proj_name;
        std::string due;
        std::vector<std::string> tags;
    };

    class TMDatabase {
    private:
        // database object for handling database operations
        sqlite3 *db_;

        /**
         * Description: gets the id of a tag in the table
         * @param[in] tag is the tag name in question, if the tag name is
         * invalid, this function will raise an error and exit
         * @return returns an int of the id of the tag found in the tags table
         */
        int tag_id(const std::string &tag);

        /**
         * Description: gets the id of a task in the table
         * @param[in] task_name: is the task name in question, if the task name
         * is invalid, this function will raise an error and exit
         * @return returns an int of the id of the task found in the tags table
         */
        int task_id(const std::string &task_name);

        /**
         * Description: gets the id of a proj in the table
         * @param[in] proj_name: is the proj name in question, if the proj name
         * is invalid, this function will raise an error and exit
         * @return returns an int of the id of the proj found in the tags table
         */
        int proj_id(const std::string &proj_name);

        /**
         * Description: returns the number of entries in a table in the database
         * @param[in] table: the name of the table to inspect
         * @return an int of the number of rows in a table, returns -1 if no table
         * is identified
         */
        int num_rows(const std::string &table);

        /**
         * Creates the tags table in the database if the tags table doesn't
         * already exist
         * The columns for the table are:
         *   id: this is an autoincremented int
         *   name: a string for the task name
         *   color: a string containing the actual text of the color, like
         *   "red", or "light-blue"
         */
        void create_tag_table();

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
        void create_task_table();


        void create_sess_table();

        /**
         * Creates a table for projects
         * The columns are:
         *   id: an autincrementing int representing a unique id for projects
         *   name: the name of the project
         *   complete: bool representing if the project is complete
         */
        void create_task_tag_table();

        /**
         * Creates a table for projects
         * The columns are:
         *   id: an autincrementing int representing a unique id for projects
         *   name: the name of the project
         *   completed: bool representing if the project is complete
         */
        void create_proj_table();

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
        void execute_query(const std::string &query,
                           const sqlite3_callback callback,
                           const std::string &err_message);

    public:
        /**
         * Description: Creates the dotfile directory for tm if it doesn't
         * already exist, then it opens an instance of a sqlite3 database for
         * storing all the data from tasks, sessions, and tags
         */
        TMDatabase();

        // Deallocates the db_ object
        ~TMDatabase();

        /**
         * Inserts a tag into the tags table
         * @param[in] tag: the tag to be inserted
         */
        void insert_tag(const Tag &tag);

        /**
         * Inserts a tag into the tags table
         * @param[in] tag: the name of the tag to be removed
         * @param[in] hard: if true, will hard remove the tag, this can be
         * specified with tm tag rm --hard
         */
        void remove_tag(const std::string &tag, bool hard);

        /**
         * Display the tags
         * @param[in] no_color: will not use color codes to print tags, useful if
         * the output of this is redirected to a file
         * @param[in] max_tags: the maximum number of tags to be displayed
         */
        void list_tags(bool no_color, int max_tags);

        /**
         * Description: Update a task to complete in the tasks table
         * @param[in] task_name: The id of the task to update to complete
         * @param[in] val: The value for complete to be set to, 0 if it should
         * be set to incomplete, 1 if complete
         */
        void complete_task(int task_id, int val);

        /**
         * Description: remove a task from the tasks table
         * @param[in] task_id: The id of the task to remove
         */
        void remove_task(int task_id);

        /**
         * Description: adds a new task to the tasks table
         * @param[in] task: the task to be inserted
         */
        void add_task(const Task &task);

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
        void list_tasks(bool list_long, int max_tasks, bool display_done,
                        const std::vector<std::string> &specified_tags,
                        const std::string &specified_date);

        // Sess related funcitons
        void sess_log(bool condensed, int max_sessions);
        void add_sess(const std::string &start,
                      const std::string &stop,
                      const std::string &task,
                      const std::string &description);

        /**
         * Description: remove a project from the projects table
         * @param[in] proj_name: The name of the project to remove
         * @param[in] hard: If true, then the project gets removed from the database, and
         * all the tasks that belong to the project have their project set to null
         * @return Returns
         */
        void remove_project(std::string proj_name, bool hard);

        /**
         * Description: Set the projects status to complete, if there are still tasks that
         * reference that project and are unfinished, then this will fail
         * @param[in] proj_name: The name of the project to complete
         */
        void complete_project(std::string proj_name);

        /**
         * Description: adds a new project to the project table
         * @param[in] proj_name: the name of the project to be added
         */
        void add_project(const std::string &proj_name);

        /**
         * Description: display a list of projects, matching the criteria
         * @param[in] show_tasks: display all the projects with their corresponding tasks
         * @param[in] display_done: if true will also display completed projects that
         * match the specified criteria, if the long option is true, displays completed
         * tasks as well from those projects
         * @param[in] proj_names: only display tasks from those specific projects, this
         * will automatically set the long option to true
         */
        void list_projects(bool show_tasks, bool display_done,
                           const std::vector<std::string> &proj_names);

    };
}

#endif // DATABASE_HPP_
