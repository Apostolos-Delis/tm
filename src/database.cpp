//
// Definitions of everything regarding the internal database that tm manages
//


#include <string>
#include <vector>
#include <sqlite3.h>

#include "database.hpp"

sqlite3 *db_;


tm_db::TMDatabase::TMDatabase() {

}


int tm_db::TMDatabase::tag_id(const std::string &tag) {
    return 0;
}


int tm_db::TMDatabase::task_id(const std::string &task_name) {
    return 0;
}

void tm_db::TMDatabase::create_tag_table() {

}


void tm_db::TMDatabase::create_task_table() {

}


void tm_db::TMDatabase::create_sess_table() {

}


void tm_db::TMDatabase::create_task_tag_table() {

}


void tm_db::TMDatabase::insert_tag(const Tag &tag) {

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

