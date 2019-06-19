//
// Implementations of the different subroutines for handling task commands
//

#include <string>
#include <vector>

#include "task.hpp"

// Removes the task completely from the database
void tm_task::handle_rm(const std::string &task_name){

}

// Updates the task from in-progress to complete
void tm_task::handle_done(const std::string &task_name) {

}


// Adds a new task into the database
// the tags get added to a seperate table, with multiple entries
// where each row of that table is task_id, tag_id
void tm_task::handle_add(const std::string &task_name,
                const std::string &due_date,
                const std::string &due_time,
                const std::vector<std::string> &tags) {

}


// Display all the current tasks
void tm_task::handle_list(bool condensed, int max_tags, bool display_done,
                 const std::vector<std::string> &specified_tags) {

}
