//
// Definitions of everything regarding the internal database that tm manages
//


#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <sys/stat.h>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sstream>


namespace tm_utils {
    /**
     * Checks if a folder exists
     * @param foldername path to the folder to check.
     * @return true if the folder exists, false otherwise.
     */
    bool folder_exists(std::string foldername);

    /**
     * Portable wrapper for mkdir. Internally used by mkdir()
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
    int _mkdir(const char *path);

    /**
     * Recursive, portable wrapper for mkdir.
     * @param[in] path the full path of the directory to create.
     * @return zero on success, otherwise -1.
     */
    int mkdir(const char *path);

    /**
     * Returns the home directory of the tm_user
     * @param[in] Param
     * @return string with the home directory of the user calling the function
     */
    std::string home_dir();

    /**
     * asserts that the date is in proper format and also valid
     * @param[in] date: a string of the form (ideally) of YYYY-MM-DD
     * @return Returns true if the date is valid, false otherwise
     */
    bool valid_date(std::string date);

    /**
     * asserts that the time is in proper format and also valid
     * @param[in] time_str: a string of the form (ideally) of HH:MM
     * @return Returns true if the date is valid, false otherwise
     */
    bool valid_time(std::string time_str);
}

// class for defining colors
namespace tm_color {

    const std::unordered_set<std::string> VALID_COLORS({
            "red", "light-red",
            "green", "light-green",
            "yellow", "light-yellow",
            "blue", "light-blue",
            "magenta", "light-magenta",
            "cyan", "light-cyan",
            "light-gray", "dark-gray",
            "black", "white"
    });

    const std::string NOCOLOR = "\033[0m";

    const std::unordered_map<std::string, std::string> COLOR_CODES = {
            {"black", "\033[30m"},
            {"red", "\033[31m"},
            {"green", "\033[32m"},
            {"yellow", "\033[33m"},
            {"blue", "\033[34m"},
            {"magenta", "\033[35m"},
            {"cyan", "\033[36m"},
            {"light-gray", "\033[37m"},
            {"dark-gray", "\033[90m"},
            {"light-red", "\033[91m"},
            {"light-green", "\033[92m"},
            {"light-yellow", "\033[93m"},
            {"light-blue", "\033[94m"},
            {"light-magenta", "\033[95m"},
            {"light-cyan", "\033[96m"},
            {"white", "\033[97m"},
    };

    const std::unordered_map<std::string, std::string> BACKGROUNDS = {
            {"black", "\033[0;90;39m"},
            {"red", "\033[0;91;39m"},
            {"green", "\033[0;92;39m"},
            {"yellow", "\033[0;93;39m"},
            {"blue", "\033[0;94;39m"},
            {"magenta", "\033[0;95;39m"},
            {"cyan", "\033[0;96;39m"},
            {"light-gray", "\033[0;97;39m"},
            {"dark-gray", "\033[0;90;39m"},
            {"light-red", "\033[0;91;39m"},
            {"light-green", "\033[0;92;39m"},
            {"light-yellow", "\033[0;93;39m"},
            {"light-blue", "\033[0;94;39m"},
            {"light-magenta", "\033[0;95;39m"},
            {"light-cyan", "\033[0;96;39m"},
            {"white", "\033[0;97;39m"},
    };
}

#endif // UTILS_HPP_
