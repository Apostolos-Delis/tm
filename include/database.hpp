//
// Definitions of everything regarding the internal database that tm manages
//

#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include <string>
#include <vector>
#include <sqlite3.h>

#include <tag.hpp>

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

    class TM_DB {
    private:
        // Object for handling 
        sqlite3 *db_;

        int tag_id(const std::string &tag);
        int task_id(const std::string &task_name);
        // There are 4 tables in total in the database
        void create_tag_table();
        void create_task_table();
        void create_sess_table();
        void create_task_tag_table();
    public:
        TM_DB();

        // Tag related functions
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
