//
// Implementations of the different subroutines for handling task commands
//

#include <iostream>
#include <string>
#include <vector>

#include "task.hpp"
#include "utils.hpp"
#include "database.hpp"


/**
 * Description: remove a task from the tasks table
 * @param[in] task_name: The name of the task to remove
 * @param[in] hard: Remove the task even if it is referenced by other
 * sessions
 */
void tm_task::handle_rm(int task_id, bool hard) {
    auto db = tm_db::TMDatabase();
    db.remove_task(task_id, hard);
}


/**
 * Description: Update a task to complete in the tasks table
 * @param[in] task_name: The name of the task to update to complete
 * @param[in] reversed: if true, set the task to be incomplete
 */
void tm_task::handle_done(int task_id, bool reversed) {
    auto db = tm_db::TMDatabase();
    if (!db.valid_task_id(task_id) && !reversed) {
        std::cerr << "'" << task_id 
                  << "' is not an id for any current incomplete task." 
                  << std::endl;
        std::cerr << "It is possible the task is already complete." << std::endl;
        std::cerr << "To check, run 'tm task list -c'" << std::endl;
        exit(1);
    }
    db.complete_task(task_id, reversed ? 0 : 1);
}


/**
 * Description: adds a new task to the tasks table
 * @param[in] task_name: the name of the task to be added
 * @param[in] proj_name: the name of the project the task is in, if the
 * task isn't in a project, this should be an empty string
 * @param[in] due_date: the date of when the task is due (format: dd-mm-YY)
 * @param[in] due_time: the time that the task is due (format: HH:MM)
 * @param[in] tags: list of tags affiliated with the task
 */
void tm_task::handle_add(const std::string &task_name,
                         const std::string &proj_name,
                         const std::string &due_date,
                         const std::string &due_time,
                         const std::vector<std::string> &tags) {
    if (!tm_utils::valid_date(due_date)) {
        std::cerr << "ERROR: '" << due_date
                  << "' is not a valid date!" << std::endl;
        exit(1);
    } else if (!tm_utils::valid_time(due_time)) {
        std::cerr << "ERROR: '" << due_time
                  << "' is not a valid time!" << std::endl;
        exit(1);
    }
    std::string due = due_date + " " + due_time + ":00.000";

    tm_db::Task task = {task_name, proj_name, due, tags};
    auto db = tm_db::TMDatabase();
    db.add_task(task);
}

/**
 * Description: display a list of tasks, matching the criteria
 * @param[in] list_long: display additional information about each task
 * @param[in] max_tasks: the maximum number of tasks to display
 * @param[in] display_done: if true will also display completed tasks that
 * match the specified criteria
 * @param[in] reversed: reverse the chronological order for when tasks are due
 * @param[in] specified tags: only display tags that have one of the tags
 * @param[in] date_till: only display tags that are due until the
 * specified date
 * @param[in] date_from: only display tags that are due after the
 * specified date
 * @param[in] specified date: only display tags that are due on the
 * specified date
 * @param[in] specified_proj: only display tags that are due in the
 * specified project
 */
void tm_task::handle_list(bool list_long, int max_tasks, bool display_done,
                 bool reversed,
                 const std::vector<std::string> &specified_tags,
                 const std::string &specified_date,
                 const std::string &date_from,
                 const std::string &date_till,
                 const std::string &specified_proj) {
    if (!specified_date.empty() && !tm_utils::valid_date(specified_date)) {
        std::cerr << "ERROR: '" << specified_date
                  << "' is not a valid date!" << std::endl;
        exit(1);
    }
    if (!date_from.empty() && !tm_utils::valid_date(date_from)) {
        std::cerr << "ERROR: '" << date_from
                  << "' is not a valid date!" << std::endl;
        exit(1);
    }
    if (!date_till.empty() && !tm_utils::valid_date(date_till)) {
        std::cerr << "ERROR: '" << date_till
                  << "' is not a valid date!" << std::endl;
        exit(1);
    }
    auto db = tm_db::TMDatabase();
    db.list_tasks(list_long, max_tasks, display_done, reversed,
                  specified_tags, specified_date, date_from, 
                  date_till, specified_proj);
}
