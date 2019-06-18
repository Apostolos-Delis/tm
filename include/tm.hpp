//
// All the defined constants and relevant project implementations found here
//
//
//

#ifndef TM_HPP_
#define TM_HPP_

#include <string>

#include "CLI11.hpp"

namespace tm_cli {
    // Current version used in the app
    std::string VERSION = "1.0, last updated: 2019-06-17";
    std::string VERSION_FLAG_DESCRIPTION = "Display tm version info";
    // Description that will be passed into a CLI::App instantiation when
    // parsing the arguments
    std::string DESCRIPTION = "tm, the task manager right in your terminal!";

    // Subcommand Descriptions
    std::string SESSION_DESCRIPTION =
        "creates a work session, for uninterrupted productivity";
    std::string TASK_DESCRIPTION =
        "add and remove tasks from your todo list";
    std::string TAG_DESCRIPTION =
        "view and modify tags, which allow for organization of tasks";
    std::string STAT_DESCRIPTION =
        "display personal statistics and progress";
}

#endif // TM_HPP_
