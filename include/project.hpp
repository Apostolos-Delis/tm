//
// All the defined constants and functions relevant to project, which are
// stored in a sqlite database inside of ~/.tm.d/data.db
//


#ifndef PROJECT_HPP_
#define PROJECT_HPP_

#include <string>
#include <vector>


namespace tm_proj {

    // Descriptions of all the possible flags and subcommands
    const std::string ADD_DESCRIPTION = "creates a new project";
    const std::string RM_DESCRIPTION = "remove an existing project";
    const std::string HARD_DESCRIPTION = "removes a project even if it is referenced by other tasks";
    const std::string DONE_DESCRIPTION = "complete an existing task";
    const std::string NAME_DESCRIPTION =
        "required flag to specify the name of the task being modified";
    const std::string ID_DESCRIPTION =
        "required flag to specify the id of the task being modified";
    const std::string SILENT_DESCRIPTION =
        "fail to remove silently, will not print anything to stderr if rm fails";
    const std::string LIST_DESCRIPTION =
        "display the current tasks used";
    const std::string MAX_DESCRIPTION =
        "the maximum number of tasks to be displayed, by default,\nthere is no max";
    const std::string CONDENSED_DESCRIPTION =
        "only show the tasks without all their tags or due dates";
    const std::string DISPLAY_DONE_DESCRIPTION =
        "display the completeted tasks along with the incomplete\nones";
    const std::string LIST_TAGS_DESCRIPTION =
        "display only tasks that have a tag that matches one\nof the tags in the specified list";
    const std::string LIST_DATE_DESCRIPTION =
        "display only tasks that occur during that date";

    /**
     * Description: remove a project from the projects table
     * @param[in] proj_id: The id of the project to remove
     * @param[in] hard: If true, then the project gets removed from the database, and
     * all the tasks that belong to the project have their project set to null
     * @return Returns
     */
    void handle_rm(int proj_id, bool hard);

    /**
     * Description: Set the projects status to complete, if there are still tasks that
     * reference that project and are unfinished, then this will fail
     * @param[in] proj_id: The id of the project to complete
     */
    void handle_done(int proj_id);

    /**
     * Description: adds a new project to the project table
     * @param[in] proj_name: the name of the project to be added
     */
    void handle_add(const std::string &proj_name);

    /**
     * Description: display a list of projects, matching the criteria
     * @param[in] show_tasks: display all the projects with their corresponding tasks
     * @param[in] display_done: if true will also display completed projects that
     * match the specified criteria, if the long option is true, displays completed 
     * tasks as well from those projects
     * @param[in] proj_ids: only display tasks from those specific projects, this
     * will automatically set the long option to true
     */
    void handle_list(bool show_tasks, bool display_done,
                     const std::vector<std::string> &proj_ids);

}

#endif // PROJECT_HPP_
