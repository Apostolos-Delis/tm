//
// Defines the session
//

#ifndef SESS_HPP_
#define SESS_HPP_

#include <string>

// Default number of sessions to display with tm sess log
#define DEFAULT_LOG_LENGTH 50

namespace tm_sess {

    // Descriptions of all the possible flags and subcommands
    const std::string START_DESCRIPTION =
        "Start a work session for a specified amount of time";

    const std::string TASK_DESCRIPTION =
        "The designated task to be worked on during the session";

    const std::string LENGTH_DESCRIPTION =
        "The length of the session (in min)";

    const std::string INTERRUPT_DESCRIPTION =
        "Do not allow the session to be terminated (with ^C)";

    const std::string OVERTIME_DESCRIPTION =
        "No overtime means the session will end immediately\nupon reaching the time limit"\
        "otherwise, the session will keep going until the\nuser stops it";

    const std::string DESC_DESCRIPTION =
        "Add a brief description you plan to do during the session";

    const std::string LOG_DESCRIPTION =
        "Display a list of sessions, with a max limit default of 50";

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

    void handle_log(bool condensed, int max_sessions);

}

#endif // SESS_HPP_
