//
// All the defined constants and relevant project implementations found here
//

#ifndef TM_HPP_
#define TM_HPP_

#include <string>

#include "CLI11.hpp"
#include "tag.hpp"
#include "task.hpp"
#include "sess.hpp"
#include "project.hpp"


namespace tm_cli {
    // Current version used in the app
    const std::string VERSION = "1.2, last updated: 2019-07-31";

    const std::string VERSION_FLAG_DESCRIPTION = "Display tm version info";

    // Description that will be passed into a CLI::App instantiation when
    // parsing the arguments
    const std::string DESCRIPTION = "tm, the task manager right in your terminal.";

    // Subcommand Descriptions
    const std::string SESSION_DESCRIPTION =
        "Creates a work session, for uninterrupted productivity";

    const std::string TASK_DESCRIPTION =
        "Add and remove tasks from your todo list";

    const std::string TAG_DESCRIPTION =
        "View and modify tags, which allow for organization of tasks";

    const std::string STAT_DESCRIPTION =
        "Display personal statistics and progress";

    const std::string PROJ_DESCRIPTION =
        "Allows for the modification of projects, which are composed of multiple tasks";
}

#endif // TM_HPP_
