//
// All the defined constants and functions relevant to tasks, which are
// stored in a sqlite database inside of ~/.tm.d/database/*
//

#ifndef TASK_HPP_
#define TASK_HPP_

#include <string>
#include <vector>

#include "CLI11.hpp"

namespace tm_task {

    // Descriptions of all the possible flags and subcommands
    std::string ADD_DESCRIPTION = "add a task to the list of tasks";
    std::string DATE_DESCRIPTION =
        "the due date of the specified task, input format must be dd/mm/yyyy";
    std::string TIME_DESCRIPTION =
        "the time that a task must be completed by (default is 23:59), format must be: HH:MM";
    std::string TAGS_DESCRIPTION =
        "list of tags that a task fits under, will fail if any tags are invalid";
    std::string RM_DESCRIPTION = "remove an existing task";
    std::string DONE_DESCRIPTION = "complete an existing task";
    std::string NAME_DESCRIPTION =
        "required flag to specify the name of the task being modified";
    std::string SILENT_DESCRIPTION =
        "fail to remove silently, will not print anything to stderr if rm fails";
    std::string LIST_DESCRIPTION =
        "display the current tasks used";
    std::string MAX_DESCRIPTION =
        "the maximum number of tasks to be displayed, by default, there is no max";
    std::string CONDENSED_DESCRIPTION =
        "only show the tasks without all their tags or due dates";

    // functions for handling the different tag subcommands
    void handle_rm(const std::string &tag_name);
    void handle_done(const std::string &tag_name);
    void handle_add(const std::string &tag_name,
                    const std::string &due_date,
                    const std::string &due_time,
                    const std::vector<std::string> &tags);
    void handle_list(bool condensed, int max_tags);

}

#endif // TASK_HPP_
