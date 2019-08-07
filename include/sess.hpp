//
// Defines the session
//

#ifndef SESS_HPP_
#define SESS_HPP_

#include <string>

// Default number of sessions to display with tm sess log
#define DEFAULT_LOG_LENGTH 50

// Default session of 25 min, based on pomodoro technique
#define DEFAULT_SESS_LENGTH 25

namespace tm_sess {

    // Descriptions of all the possible flags and subcommands
    const std::string START_DESCRIPTION =
        "Start a work session for a specified amount of time";

    const std::string TASK_DESCRIPTION =
        "The designated task to be worked on during the session";

    const std::string LENGTH_DESCRIPTION =
        "The length of the session (in min), maximum is 3 hours (180 min)";

    const std::string INTERRUPT_DESCRIPTION =
        "Do not allow the session to be terminated (with ^C)";

    const std::string OVERTIME_DESCRIPTION =
        "No overtime means the session will end immediately\nupon reaching the time limit"\
        " otherwise, the session will keep going until the\nuser stops it";

    const std::string DESC_DESCRIPTION =
        "Add a brief description you plan to do during the session";

    const std::string ADD_DESCRIPTION =
        "Adds a sess to the system that occured in the past";

    const std::string DATE_DESCRIPTION =
        "The date that the session was started on\n"
        "Format: YYYY-MM-DD";

    const std::string TIME_DESCRIPTION =
        "The time that the session was started on\n"
        "Format: HH:MM";

    const std::string RM_DESCRIPTION = "Removes a session from the system";

    const std::string ID_DESCRIPTION = "The id of the session to be removed";

    const std::string LOG_DESCRIPTION =
        "Display a list of sessions, with a max limit default of 50";

    const std::string REVERSED_DESCRIPTION =
        "Display the sessions in reversed chronological order";

    const std::string ALL_DESCRIPTION =
        "Effectively the same as removing the max restriction\n"
        "if -a and -m N options are selected, then -a will overwite -m";

    const std::string MAX_DESCRIPTION =
        "Maximum number of sessions to display in the log; \n0 means all of them (slow)";

    const std::string CONDENSED_DESCRIPTION =
        "Display a minimal log of all the sessions";

    // functions for handling the different session subcommands
    /**
     * Description: starts a session
     * @param[in] sess_length: the amount of time, in min for a session
     * @param[in] no_interrupt: prevents the session from terminating with ^C
     * @param[in] no_overtime: if true, the session ends immediately after the
     * time is over, otherwise it continues for up to 50% more time
     * @param[in] task_id: the task id of the task being worked on
     * @param[in] description: A quick description of the session's goals
     */
    void handle_start(int sess_length, bool no_interrupt,
                      bool no_overtime, int task_id,
                      const std::string &description);


    /**
     * Description: Displays a log of all the recent sessions
     * @param[in] condensed: If condensed, the log only shows the time and duration of the log,
     * as well as the task_id, otherwise, the log is similar in style to 'git log'
     * @param[in] max_sessions: The max number of sessions to display, default is defined by
     * DEFAULT_LOG_LENGTH
     * @param[in] reversed: Display the sessions in reversed chronological order if true
     */
    void handle_log(bool condensed, int max_sessions, bool reversed);

    /**
     * Description: removes a session from the sess table
     * @param[in] sess_id, the id of the session to remove
     */
    void handle_remove(int sess_id);

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
    void handle_add(int sess_length, int task_id,
                             const std::string start_date,
                             const std::string start_time,
                             const std::string &description);
}

#endif // SESS_HPP_
