//
// All the defined constants and functions relevant to tasks, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//

#ifndef TASK_HPP_
#define TASK_HPP_

#include <string>
#include <vector>


namespace tm_task {

    // Descriptions of all the possible flags and subcommands
    const std::string ADD_DESCRIPTION = "add a task to the list of tasks";
    const std::string DATE_DESCRIPTION =
        "the due date of the specified task, input format must be dd/mm/yyyy";
    const std::string TIME_DESCRIPTION =
        "the time that a task must be completed by (default is 23:59),\nformat must be: HH:MM";
    const std::string TAGS_DESCRIPTION =
        "list of tags that a task fits under, will fail if any tags are invalid";
    const std::string RM_DESCRIPTION = "remove an existing task";
    const std::string DONE_DESCRIPTION = "complete an existing task";
    const std::string NAME_DESCRIPTION =
        "required flag to specify the name of the task being modified";
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
     * Description: remove a task from the tasks table
     * @param[in] task_name: The name of the task to remove
     * @return Returns
     */
    void handle_rm(const std::string &task_name);

    /**
     * Description: Update a task to complete in the tasks table
     * @param[in] task_name: The name of the task to update to complete
     */
    void handle_done(const std::string &task_name);

    /**
     * Description: adds a new task to the tasks table
     * @param[in] task_name: the name of the task to be added
     * @param[in] due_date: the date of when the task is due (format: dd-mm-YY)
     * @param[in] due_time: the time that the task is due (format: HH:MM)
     * @param[in] tags: list of tags affiliated with the task
     */
    void handle_add(const std::string &task_name,
                    const std::string &due_date,
                    const std::string &due_time,
                    const std::vector<std::string> &tags);

    /**
     * Description: display a list of tasks, matching the criteria
     * @param[in] condensed: display all the tasks with a minimized output
     * @param[in] max_tasks: the maximum number of tasks to display
     * @param[in] display_done: if true will also display completed tags that
     * match the specified criteria
     * @param[in] specified tags: only display tags that have one of the tags
     * @param[in] specified date: only display tags that are due on the
     * specified date
     */
    void handle_list(bool condensed, int max_tasks, bool display_done,
                     const std::vector<std::string> &specified_tags,
                     const std::string &specified_date);

}

#endif // TASK_HPP_
