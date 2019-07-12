//
// Implementations of the different subroutines for handling task commands
//

#include <string>
#include <vector>

#include "project.hpp"
#include "database.hpp"


/**
 * Description: remove a project from the projects table
 * @param[in] proj_id: The id of the project to remove
 * @param[in] hard: If true, then the project gets removed from the database, and
 * all the tasks that belong to the project have their project set to null
 * @return Returns
 */
void tm_proj::handle_rm(int proj_id, bool hard) {
    auto db = tm_db::TMDatabase();
    db.remove_project(proj_id, hard);
}

/**
 * Description: Set the projects status to complete, if there are still tasks that
 * reference that project and are unfinished, then this will fail
 * @param[in] proj_id: The id of the project to complete
 */
void tm_proj::handle_done(int proj_id) {
    auto db = tm_db::TMDatabase();
    db.complete_project(proj_id);
}

/**
 * Description: adds a new project to the project table
 * @param[in] proj_name: the name of the project to be added
 */
void tm_proj::handle_add(const std::string &proj_name) {
    auto db = tm_db::TMDatabase();
    db.add_project(proj_name);
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
void tm_proj::handle_list(bool show_tasks, bool display_done,
                          const std::vector<std::string> &proj_ids) {
    auto db = tm_db::TMDatabase();
    db.list_projects(show_tasks, display_done, proj_ids);
}

