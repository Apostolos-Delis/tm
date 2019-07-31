//
// Definitions of everything regarding the internal database that tm manages
//

#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h> 
#include <ctime>
#include <pwd.h>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>


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
    if (!(1 <= m && m <= 12)) {
        return false;
    }
    if (!(1 <= d && d <= 31)) {
        return false;
    }
    if ((d == 31) && (m == 2 || m == 4 || m == 6 || m == 9 || m == 11)) {
        return false;
    }
    if ((d == 30) && (m == 2)) {
        return false;
    }
    if ((m == 2) && (d == 29) && (y % 4 != 0)) {
        return false;
    }
    if ((m == 2) && (d == 29) && (y % 400 == 0)) {
        return true;
    }
    if ((m==2) && (d==29) && (y%100==0)) {
        return false;
    }
    if ((m==2) && (d==29) && (y%4==0)) {
        return true;
    }
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


/**
 * Description: returns a string of the current date and time as a string of the with the
 * following format:
 * YYYY-MM-DD HH:MM
 * Reference: https://stackoverflow.com/questions/16357999/current-date-and-time-as-string
 */
std::string tm_utils::current_datetime() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[30];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M",timeinfo);
    std::string ret(buffer);

    return ret;
}


/**
 * Description: creates a string out of the number of seconds 
 * @param[in] num_seconds: the number of seconds to convert
 * @return returns a string of the form H:MM:SS
 */
std::string tm_utils::sec_to_time(int num_seconds) {
    int num_hours = num_seconds / 3600;
    if (num_hours) {
        num_seconds %= (num_hours * 3600);
    }
    int num_min = num_seconds / 60;
    if (num_min) {
        num_seconds %= (num_min * 60);
    }
    std::stringstream ss;
    ss << num_hours << ":" << std::setw(2) << std::setfill('0')
       << num_min << ":" << std::setw(2) << std::setfill('0')
       << num_seconds;
    return ss.str();
}

/**
 * Description: writes string to file
 * @param[in] fname: the name of the file
 * @param[in] input: the input string
 */
void tm_utils::write_to_file(std::string fname, std::string input) {
#ifndef _WIN32
    std::string cmd = "touch " + fname;
    system(cmd.c_str());
#endif
    std::ofstream log_file; 
    log_file.open(fname, std::ios_base::app);
    log_file << input;
    log_file.close();
}

/**
 * Description: removes a file (only for UNIX)
 * @param[in] fname: the name of the file to remove
 */
void tm_utils::remove_file(std::string fname) {
#ifndef _WIN32
    if(remove(fname.c_str()) != 0) {
        std::cerr << "ERROR: Failed to remove file: '"
                  << fname << "'" << std::endl;
        exit(1);
    }
#endif
}
