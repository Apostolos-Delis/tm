//
// Definitions of everything regarding the internal database that tm manages
//


#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <sys/stat.h>
#include <sys/ioctl.h> 
#include <unistd.h>

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>


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

    /**
     * Description: returns a string of the current date and time as a string of the with the
     * following format:
     * YYYY-MM-DD HH:MM
     */
    std::string current_datetime();

    /**
     * Description: creates a string out of the number of seconds 
     * @param[in] num_seconds: the number of seconds to convert
     * @return returns a string of the form HH:MM:SS
     */
    std::string sec_to_time(int num_seconds);

    /**
     * Description: writes string to file
     * @param[in] file_name: the name of the file
     * @param[in] input: the input string
     */
    void write_to_file(std::string file_name, std::string input);

    /**
     * Description: create an empty file
     * @param[in] file_name: the name of the file to create
     */
    void make_file(std::string file_name);

    /**
     * Description: removes a file
     * @param[in] file_name: the name of the file to remove
     */
    void remove_file(std::string file_name);

    /**
     * Description: Returns the number of columns in the terminal
     */
    inline int num_cols() {
#ifndef _WIN32
        struct winsize size;
        ioctl(STDOUT_FILENO,TIOCGWINSZ,&size);
        return size.ws_col;
#else
#include <windows.h>
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        return columns;
#endif
    }
}

namespace tm_math {

    /**
     * Description: Computes the mean of the vector of numbers
     * @param[in] nums: the vector of numbers
     * @return returns the average of the values, in the same numeric type as
     * recieved, so if nums was a vector of ints, the average will be an int
     */
    template <typename NumericType>
    NumericType mean(std::vector<NumericType> nums) {
        // Check to make sure that the NumericType is arithmentic
        static_assert(std::is_arithmetic<NumericType>::value,
                      "NumericType must be numeric");

        NumericType sum = 0;
        for (auto const& it : nums) {
            sum += it;
        }
        return sum / (NumericType) nums.size();
    }


    /**
     * Description: Takes in a vector of numeric types, and then returns the
     * standard deviation, requires the mean to have been precomputed
     * @param[in] nums: vector of nums
     * @param[in] mean: the mean of the vector
     * @return a floating point value of the precomputed value
     */
    template <typename NumericType>
    double standard_dev(std::vector<NumericType> nums, NumericType mean) {
        static_assert(std::is_arithmetic<NumericType>::value,
                      "NumericType must be numeric");
        double sum = 0.0;
        for (auto const &it : nums) {
            sum += pow((it - mean), 2);
        }
        return sqrt(sum / nums.size());
    }


// This is used in case the max and min values are the same, so
// that we don't divide by 0
#define ERR_VAL 0.001

    /**
     * Description: Normalizes a value to be between 0 and 1
     * @param[in] val: the value to be normalized
     * @param[in] min_val: the minimum value from the set of values
     * @param[in] max_val: the maximum value from the set of values
     * @return (val - min) / (max - min)
     */
    template <typename NumericType>
    inline double normalize(NumericType val, NumericType min_val,
                            NumericType max_val) {

        static_assert(std::is_arithmetic<NumericType>::value,
                      "NumericType must be numeric");

        double max_min = max_val - min_val + ERR_VAL;
        return (val - min_val + ERR_VAL) / max_min;
    }
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
            "black", "white", "none"
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
            {"none", ""}
    };

    const std::unordered_set<std::string> VALID_GRADIENTS({
            "orange", "gray", "green", "blue", "magenta"
    });

    const std::unordered_map<std::string, 
          std::vector<int>> GRADIENTS = {
        {"blue", {87, 51, 45, 39, 33}},
        {"green", {118, 82, 46, 40, 34}},
        {"magenta", {225, 219, 213, 201, 165}},
        {"orange", {220, 214, 208, 202, 196}},
        {"gray", {255, 250, 244, 238, 232}},
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
            {"white", "\033[0;97;39m"}
    };
}

#endif // UTILS_HPP_
