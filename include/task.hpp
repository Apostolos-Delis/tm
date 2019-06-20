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


    // functions for handling the different task subcommands
    void handle_rm(const std::string &task_name);

    void handle_done(const std::string &task_name);

    void handle_add(const std::string &task_name,
                    const std::string &due_date,
                    const std::string &due_time,
                    const std::vector<std::string> &tags);

    void handle_list(bool condensed, int max_tags, bool display_done,
                     const std::vector<std::string> &specified_tags,
                     const std::string &specified_date);

}

#endif // TASK_HPP_
