//
// All the defined constants and functions relevant to tasks, which are
// stored in a sqlite database inside of ~/.tm.d/data.db
//

#ifndef TASK_HPP_
#define TASK_HPP_

#include <string>
#include <vector>


namespace tm_task {

    // Descriptions of all the possible flags and subcommands
    const std::string ADD_DESCRIPTION = "Add a task to the list of tasks";

    const std::string DATE_DESCRIPTION =
        "The due date of the specified task, input format must be dd/mm/yyyy";

    const std::string PROJ_DESCRIPTION =
        "The name of the project the task is part of, by default a task isn't in a project";

    const std::string TIME_DESCRIPTION =
        "The time that a task must be completed by (default is 00:00),\nformat must be: HH:MM"
        " and the values must\nrange from 00:00 - 23:59";

    const std::string TAGS_DESCRIPTION =
        "List of tags that a task fits under, will fail if any tags are invalid";

    const std::string RM_DESCRIPTION = "Remove an existing task";

    const std::string DONE_DESCRIPTION = "Complete an existing task";

    const std::string REVERSED_DESCRIPTION =
        "Marks an existing task as incomplete";

    const std::string NAME_DESCRIPTION =
        "Required flag to specify the name of the task being modified";

    const std::string ID_DESCRIPTION =
        "Required flag to specify the id of the task being modified";

    const std::string SILENT_DESCRIPTION =
        "Fail to remove silently, will not print anything to stderr if rm fails";

    const std::string LIST_DESCRIPTION =
        "Display the current tasks used";

    const std::string MAX_DESCRIPTION =
        "The maximum number of tasks to be displayed, by default,\nthere is no max";

    const std::string LIST_LONG_DESCRIPTION =
        "Also display the tags of each task, and what project the task belongs to";

    const std::string DISPLAY_DONE_DESCRIPTION =
        "Display the completeted tasks along with the incomplete\nones";

    const std::string LIST_TAGS_DESCRIPTION =
        "Display only tasks that have a tag that matches one\nof the tags in the specified list";

    const std::string LIST_DATE_DESCRIPTION =
        "Display only tasks that occur during that date";

    /**
     * Description: remove a task from the tasks table
     * @param[in] task_id: The id of the task to remove
     * @return Returns
     */
    void handle_rm(int task_id);

    /**
     * Description: Update a task to complete in the tasks table
     * @param[in] task_id: The id of the task to update to complete
     * @param[in] reversed: if true, set the task to be incomplete
     */
    void handle_done(int task_id, bool reversed);

    /**
     * Description: adds a new task to the tasks table
     * @param[in] task_name: the name of the task to be added
     * @param[in] proj_name: the name of the project the task is in, if the
     * task isn't in a project, this should be an empty string
     * @param[in] due_date: the date of when the task is due (format: dd-mm-YY)
     * @param[in] due_time: the time that the task is due (format: HH:MM)
     * @param[in] tags: list of tags affiliated with the task
     */
    void handle_add(const std::string &task_name,
                    const std::string &proj_name,
                    const std::string &due_date,
                    const std::string &due_time,
                    const std::vector<std::string> &tags);

    /**
     * Description: display a list of tasks, matching the criteria
     * @param[in] list_long: display additional information about each task
     * @param[in] max_tasks: the maximum number of tasks to display
     * @param[in] display_done: if true will also display completed tasks that
     * match the specified criteria
     * @param[in] specified tags: only display tags that have one of the tags
     * @param[in] specified date: only display tags that are due on the
     * specified date
     */
    void handle_list(bool list_long, int max_tasks, bool display_done,
                     const std::vector<std::string> &specified_tags,
                     const std::string &specified_date);

}

#endif // TASK_HPP_
