//
// Defines the tm stat
//

#ifndef STAT_HPP_
#define STAT_HPP_


#include <string>

namespace tm_stat {

    // Descriptions of all the possible flags and subcommands
    const std::string SUM_DESCRIPTION =
        "Get a summary of statistics related to tm";

    const std::string TASK_DESCRIPTION =
        "Statistics will be related to tasks";

    const std::string ALL_DESCRIPTION =
        "Get statistics without regard for time period, by default,\n"
        "'tm stat sum' will query for data from the past 365 days";

    const std::string YEAR_DESCRIPTION =
        "Specify the year for which statistics to be focused on"
        "\nFormat: 'YYYY'";

    const std::string FROM_DESCRIPTION =
        "Only summarize statistics from data after this date"
        "\nFormat: 'YYYY'";

    const std::string UNTIL_DESCRIPTION =
        "Only summarize statistics from data up to this date"
        "\nFormat: 'YYYY'";

    /**
     * Description: prints a summar
     * @param[in] sum_tasks: print a summary related to tasks, not about time
     * @param[in] sum_all: summary is not just about the previous 365 days, it 
     * every datapoint, regardless of date
     * @param[in] year: return summary statistics about that year
     * @param[in] from: statistics are restricted to data after from date
     * @param[in] until: statistics restricted to data before until date
     *
     * Note: year, from, and until must be a 4 digit number
     */
    void handle_summary(bool sum_tasks, bool sum_all,
                        const std::string &year,
                        const std::string &from,
                        const std::string &until);

}

#endif // STAT_HPP_
