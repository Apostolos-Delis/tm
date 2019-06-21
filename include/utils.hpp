//
// Definitions of everything regarding the internal database that tm manages
//


#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>


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
}

// class for defining colors
namespace tm_color {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_WHITE    = 47,
    };
    class Color {
    private:
        Code code_;
    public:
        Color(Code pCode) : code_(pCode) {}
        friend std::ostream&

        /**
         * modifies an output stream with the appropriate ansi code coloring
         * @param[out] os: the outout stream
         * @param[in] mod: the input Color object
         * @return zero on success, otherwise -1.
         */
        operator<<(std::ostream& os, const Color& mod) {
            return os << "\033[" << mod.code_ << "m";
        }
    };
}

#endif // UTILS_HPP_
