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
    const std::string NAME_DESCRIPTION =
        "Required flag to specify the name of the proj being modified";

    const std::string ADD_DESCRIPTION = "creates a new project";

    const std::string RM_DESCRIPTION = "Remove an existing project";

    const std::string HARD_DESCRIPTION =
        "removes a project even if it is referenced by other tasks";

    const std::string DONE_DESCRIPTION = "Complete an existing proj";

    const std::string REVERSED_DESCRIPTION =
        "Sets the project to be incomplete if complete";

    const std::string LIST_DESCRIPTION = "List all the current projects";


    const std::string LONG_DESCRIPTION =
        "Display the tasks along with each project";

    const std::string DISPLAY_DONE_DESCRIPTION =
        "Display completed projects as well, if -l is selected, then\n"
        "completed tasks also get displayed";

    /**
     * Description: remove a project from the projects table
     * @param[in] proj_name: The name of the project to remove
     * @param[in] hard: Project gets removed from the database, and
     * all the tasks that belong to the project have their project set to null
     */
    void handle_rm(std::string proj_name, bool hard);

    /**
     * Description: Set the projects status to complete, if there are still
     * tasks that reference that project and are unfinished, raises error
     * @param[in] proj_name: The name of the project to complete
     * @param[in] reversed: If true, the project will be set to incomplete status
     */
    void handle_done(std::string proj_name, bool reversed);

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
     * @param[in] proj_names: only display tasks from those specific projects, this
     * will automatically set the long option to true
     */
    void handle_list(bool show_tasks, bool display_done,
                     const std::vector<std::string> &proj_names);

}

#endif // PROJECT_HPP_
