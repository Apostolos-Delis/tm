//
// Implementations of the different subroutines for handling sess commands
//

#include <signal.h>

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

#include "sess.hpp"
#include "database.hpp"
#include "utils.hpp"

// Reference: https://stackoverflow.com/questions/3437404/min-and-max-in-c
#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define MAX(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN_BAR_LENGTH 10
#define TEXT_LEN 40

// The length of the progress bar
#define BAR_WIDTH (MAX(tm_utils::num_cols() - TEXT_LEN, \
            MIN_BAR_LENGTH))

// Maximum session length is 3 hours
#define MAX_SESS_LENGTH 180


// Global variables relating to the session, allows for access within
// functions
static int len = 0;
static int sess_task_id;
static std::string sess_desc;
static std::string start;

static bool nointerrupt;

/**
 * Description: Generates a progress bar
 * @param[in] progress: a double between 0 and 1.0, indicating
 * a percentage of progress
 * @param[in] bar_width: the number of characters on the screen for
 * the bar to take up
 */
void print_bar(double progress, int bar_width) {
    if (bar_width < MIN_BAR_LENGTH) {
        return;
    }
    std::cout << "[";
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos || (i == pos && progress >= 1.0)) std::cout << "▦";
        else if (i == pos) std::cout << "▶";
        else std::cout << " ";
    }
    std::cout << "]  " << std::setw(2) << std::setfill(' ') 
        << int(progress * 100.0) << "% ";
}


/**
 * Description: handle ^C exception
 */
void handle_signal(int sig) {
    if (!nointerrupt) {
        // This newline makes sure that the progress bar prints correctly
        std::cout << std::endl;

        // Exclude sessions less than 60 seconds from the database
        if (len > 60) {
            auto db = tm_db::TMDatabase();
            db.add_sess(start, len, sess_task_id, sess_desc);
        }
        exit(0);
    } else {
        std::cerr << "\n-i option has been selected, cannot exit until time"
                  << " is up" << std::endl;
    }
}


/**
 * Description: starts a session
 * @param[in] sess_length: the amount of time, in min for a session
 * @param[in] no_interrupt: prevents the session from terminating with ^C
 * @param[in] no_overtime: if true, the session ends immediately after the
 * time is over, otherwise it continues for up to 50% more time
 * @param[in] task_id: the task id of the task being worked on
 * @param[in] description: A quick description of the session's goals
 */
void tm_sess::handle_start(int sess_length, bool no_interupt,
                           bool no_overtime, int task_id,
                           const std::string &description) {
    // Set the global variable to the current start time
    start = tm_utils::current_datetime() + ":00.000";

    if (sess_length <= 0) {
        std::cerr << "ERROR: '" << sess_length << "' is an invalid sess length"
                  << std::endl;
        std::cerr << "The length of the sess must be a positive integer from 1-180"
                  << std::endl;
    }

    if (sess_length > MAX_SESS_LENGTH) {
        std::cerr << "Session exceeds maximum limit" << std::endl;
        std::cerr << "Capping session time to " << MAX_SESS_LENGTH
                  << " minutes." <<  std::endl;
        sess_length = MAX_SESS_LENGTH;
    }

    // Assert that the id belongs to an incomplete existing task
    auto db = tm_db::TMDatabase();
    if (!db.valid_task_id(task_id)) {
        std::cerr << "ERROR: '" << task_id
                  << "' is not an id for any current incomplete task."
                  << std::endl;
        std::cerr << "Run 'tm task list --all' to see all current"
                  << " incomplete tasks" << std::endl;
        exit(1);
    }
    sess_task_id = task_id;
    nointerrupt = no_interupt;
    sess_desc = description;

    double progress = 0.0;
    // multiply by 60 since sess_length is in seconds
    int temp_seconds = sess_length * 60;

    // Initialize mechanism to catch signal
    signal(SIGINT, handle_signal);

    // Initialize the progress bar with zeros
    print_bar(0.0, BAR_WIDTH);
    std::cout << "Time Left: "
              << tm_utils::sec_to_time(temp_seconds) << " (H:MM:SS)\r";
    std::cout.flush();

    double increment = static_cast<double>(1.0 / (sess_length * 60));
    while (progress <= 1.0 && temp_seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        --temp_seconds;
        ++len;
        // This is to make sure that progress stops at 100% and doesn't go below
        // or above due to wierd floating point rounding errors
        if (!temp_seconds) {
            progress = 1.0;
        } else {
            progress += increment;
        }
        progress = MIN(progress, 1.01);

        // Update progress bar
        print_bar(progress, BAR_WIDTH);
        std::cout << "Time Left: "
                  << tm_utils::sec_to_time(temp_seconds) << " (H:MM:SS)\r";
        std::cout.flush();
    }
    std::cout << std::endl;
    // TODO (30/07/2019): Add an alarm to indicate to the user the session
    // is complete

    if (no_overtime) {
        db.add_sess(start, len, sess_task_id, sess_desc);
        return;
    }
    int max_overtime = sess_length * 30;
    // Allow the user to exit out of overtime
    nointerrupt = false;

    std::cout << "Overtime amount: "
              << tm_utils::sec_to_time(0) << " \r";
    std::cout.flush();
    for (int i = 0; i < max_overtime; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ++len;
        std::cout << "Overtime amount: "
                  << tm_utils::sec_to_time(i+1) << " \r";
        std::cout.flush();
    }
    std::cout << std::endl;
    std::cout << "Overtime limit reached, session closed with a duration "
              << "of: " << tm_utils::sec_to_time(len) << std::endl;
    db.add_sess(start, len, sess_task_id, sess_desc);
}


/**
 * Description: Displays a log of all the recent sessions
 * @param[in] condensed: If condensed, the log only shows the time and duration of the log,
 * as well as the task_id, otherwise, the log is similar in style to 'git log'
 * @param[in] max_sessions: The max number of sessions to display, default is defined by
 * DEFAULT_LOG_LENGTH
 * @param[in] reversed: Display the sessions in reversed chronological order if true
 */
void tm_sess::handle_log(bool condensed, int max_sessions, bool reversed) {
    auto db = tm_db::TMDatabase();
    db.sess_log(condensed, max_sessions, reversed);
}


/**
 * Description: removes a session from the sess table
 * @param[in] sess_id, the id of the session to remove
 */
void tm_sess::handle_remove(int sess_id) {
    auto db = tm_db::TMDatabase();
    db.remove_sess(sess_id);
}


/**
 * Description: adds a session to the database (this is in case the user 
 * worked for a certain period of time, but forgot to log the session)
 * @param[in] sess_length: the length, in min of the session to add, up to a
 * maximum of 4.5 hours (270 min)
 * @param[in] task_id: the task id of the task worked on during the session
 * @param[in] start_date: a string containing the start date of the session
 * Format: YYYY-MM-DD
 * @param[in] start_time: the start time of the sequence
 * Format: HH:MM
 * @param[in] description: the session description
 * 
 * NOTE: the combination of the date and the start time, must not be in the future,
 * you can only add sessions that 'in theory' have already happened
 */
void tm_sess::handle_add(int sess_length, int task_id,
                         const std::string start_date,
                         const std::string start_time,
                         const std::string &description) {
    auto db = tm_db::TMDatabase();
    if (sess_length > 270 || sess_length <= 0) {
        std::cerr << "ERROR: the length of the session must be a"
                  << " positive integer from 1 - 270" << std::endl;
        exit(1);
    }
    if (!tm_utils::valid_date(start_date)) {
        std::cerr << "ERROR: '" << start_date 
                  << "' is not valid date." << std::endl;
        exit(1);
    }
    if (!tm_utils::valid_time(start_time)) {
        std::cerr << "ERROR: '" << start_time 
                  << "' is not valid time." << std::endl;
        exit(1);
    }
    std::string date = start_date + " " + start_time;
    if (date > tm_utils::current_datetime()) {
        std::cerr << "ERROR: cannot pick a date in the future." << std::endl;
        exit(1);
    }
    db.add_sess(date + ":00.000", sess_length * 60, 
                task_id, description);

}
