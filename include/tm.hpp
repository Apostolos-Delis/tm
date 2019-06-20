//
// All the defined constants and relevant project implementations found here
//

#ifndef TM_HPP_
#define TM_HPP_

#include <string>

#include <sstream>
#include <sys/stat.h>

// for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif


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


namespace utils
{
    /**
     * Checks if a folder exists
     * @param foldername path to the folder to check.
     * @return true if the folder exists, false otherwise.
     */
    bool folder_exists(std::string foldername)
    {
        struct stat st;
        stat(foldername.c_str(), &st);
        return st.st_mode & S_IFDIR;
    }

    /**
     * Portable wrapper for mkdir. Internally used by mkdir()
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
    int _mkdir(const char *path)
    {
    #ifdef _WIN32
        return ::_mkdir(path);
    #else
    #if _POSIX_C_SOURCE
        return ::mkdir(path);
    #else
        return ::mkdir(path, 0755);
    #endif
    #endif
    }

    /**
     * Recursive, portable wrapper for mkdir.
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
    int mkdir(const char *path)
    {
        std::string current_level = "";
        std::string level;
        std::stringstream ss(path);

        // split path using slash as a separator
        while (std::getline(ss, level, '/'))
        {
            current_level += level; // append folder to the current level
            // create current level
            if (!folder_exists(current_level) && _mkdir(current_level.c_str()) != 0)
                return -1;
            current_level += "/"; // don't forget to append a slash
        }
        return 0;
    }
}

#endif // TM_HPP_
