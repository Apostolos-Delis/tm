//
// Definitions of everything regarding the internal database that tm manages
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
        std::string due_date;
        std::string due_time;
        std::vector<std::string> tags;
    };

    class TMDatabase {
    private:
        // Object for handling
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
         * @param[in] task_name: is the task name in question, if the tag name
         * is invalid, this function will raise an error and exit
         * @return returns an int of the id of the tag found in the tags table
         */
        int task_id(const std::string &task_name);

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

        void create_task_table();
        void create_sess_table();
        void create_task_tag_table();
    public:
        /**
         * Description: Creates the dotfile directory for tm if it doesn't
         * already exist, then it opens an instance of a sqlite3 database for
         * storing all the data from tasks, sessions, and tags
         */
        TMDatabase();

        // Deallocates the db_ object
        ~TMDatabase();

        void insert_tag(const Tag &tag);
        void remove_tag(const std::string &tag);
        void list_tags(bool no_color, int max_tags);

        // Task related functions
        void complete_task(const std::string &task_name);
        void remove_task(const std::string &task);
        void add_task(const Task &task);
        void list_tasks(bool condensed, int max_tags, bool display_done,
                        const std::vector<std::string> &specified_tags,
                        const std::string &specified_date);

        // Sess related funcitons
        void sess_log(bool condensed, int max_sessions);
        void add_sess(const std::string &start,
                      const std::string &stop,
                      const std::string &task,
                      const std::string &description);
    };
}

#endif // DATABASE_HPP_