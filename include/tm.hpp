//
// All the defined constants and relevant project implementations found here
//

#ifndef TM_HPP_
#define TM_HPP_

#include <string>

namespace tm_cli {
    // Current version used in the app
    const std::string VERSION = "1.0, last updated: 2019-06-17";
    const std::string VERSION_FLAG_DESCRIPTION = "Display tm version info";
    // Description that will be passed into a CLI::App instantiation when
    // parsing the arguments
    const std::string DESCRIPTION = "tm, the task manager right in your terminal!";

    // Subcommand Descriptions
    const std::string SESSION_DESCRIPTION =
        "creates a work session, for uninterrupted productivity";
    const std::string TASK_DESCRIPTION =
        "add and remove tasks from your todo list";
    const std::string TAG_DESCRIPTION =
        "view and modify tags, which allow for organization of tasks";
    const std::string STAT_DESCRIPTION =
        "display personal statistics and progress";
}

#endif // TM_HPP_
