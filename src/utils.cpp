//
// Definitions of everything regarding the internal database that tm manages
//

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h> 
#include <pwd.h>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>


 // for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif

#include "utils.hpp"


/**
* Checks if a folder exists
 * @param foldername path to the folder to check.
 * @return true if the folder exists, false otherwise.
 */
bool tm_utils::folder_exists(std::string foldername) {
    struct stat st;
    stat(foldername.c_str(), &st);
    return st.st_mode & S_IFDIR;
}

/**
 * Portable wrapper for mkdir. Internally used by mkdir()
 * @param[in] path the full path of the directory to create.
 * @return zero on success, otherwise -1.
 */
int tm_utils::_mkdir(const char *path)
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
int tm_utils::mkdir(const char *path) {
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


/**
 * Returns the home directory of the tm_user
 * @param[in] Param
 * @return string with the home directory of the user calling the function
 */
std::string tm_utils::home_dir() {
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    return homedir;
}


/**
 * Description: checks to see if the date specified is valid
 * @param[in] m: the month, returned as a number from 1-12
 * @param[in] d: the day, returned as a number from 1-31
 * @param[in] y: the yeay, returned as a number from 1-12
 * @return returns true if the date is valid
 */
bool checkdate(int m, int d, int y) {
  if (!(1 <= m && m <= 12))
     return false;
  if (!(1 <= d && d <= 31))
     return false;
  if ((d == 31) && (m == 2 || m == 4 || m == 6 || m == 9 || m == 11))
     return false;
  if ((d == 30) && (m == 2))
     return false;
  if ((m == 2) && (d == 29) && (y % 4 != 0))
     return false;
  if ((m == 2) && (d == 29) && (y % 400 == 0))
     return true;
  if ((m==2) && (d==29) && (y%100==0))
     return false;
  if ((m==2) && (d==29) && (y%4==0))
     return true;
  return true;
}

/**
 * asserts that the date is in proper format and also valid
 * @param[in] date: a string of the form (ideally) of YYYY-MM-DD
 * @return Returns true if the date is valid, false otherwise
 */
bool tm_utils::valid_date(std::string date) {
    if (date.length() != 10) {
        return false;
    }
    size_t i = 0;
    for (i = 0; i < 4; ++i) {
        if (!isdigit(date[i])) {
            return false;
        }
    }
    int year = stoi(date.substr(0, 4));
    if (date[4] != '-' && date[7] != '-') {
        return false;
    }
    if (!isdigit(date[5]) && !isdigit(date[6])) {
        return false;
    }
    int month = stoi(date.substr(5, 2));
    if (!isdigit(date[8]) && !isdigit(date[9])) {
        return false;
    }
    int day = stoi(date.substr(8, 2));
    return checkdate(month, day, year);
}


/**
 * asserts that the time is in proper format and also valid
 * @param[in] time_str: a string of the form (ideally) of HH:MM
 * @return Returns true if the date is valid, false otherwise
 */
bool tm_utils::valid_time(std::string time_str) {
    if (time_str.length() != 5) {
        return false;
    }
    if (!isdigit(time_str[0]) && !isdigit(time_str[1])) {
        return false;
    }
    int hour = stoi(time_str.substr(0,2));
    if (!(0 <= hour && hour < 24)) {
        return false;
    }
    if (time_str[2] != ':') {
        return false;
    }
    if (!isdigit(time_str[3]) && !isdigit(time_str[4])) {
        return false;
    }
    int minute = stoi(time_str.substr(3,2));
    if (!(0 <= minute && minute < 60)) {
        return false;
    }
    return true;
}
