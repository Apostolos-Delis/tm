//
// Defines the tm stat
//

#ifndef STAT_HPP_
#define STAT_HPP_


#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


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
     * Stats returned: Mean, Stdev, Total num
     */
    void handle_summary(bool sum_tasks, bool sum_all,
                        const std::string &year,
                        const std::string &from,
                        const std::string &until);

    /**
     * Description: Class for handling statistics processing
     */
    class StatHandler {
    private:
        // a map that maps dates -> values
        std::unordered_map<std::string, double> data_;

        // A list of the values found in data_
        std::vector<double> vals_;

        // Specifies the starting date and final date in the dataset
        std::string min_date, max_date;

        /**
         * Description: loads all the values from data_ into vals_
         */
        void load_vals();
    public:

        /**
         * @param[in] data: a map that maps dates -> values
         */
        StatHandler(const std::unordered_map<std::string, double> &data);

        /**
         * Description: Outputs a summary about the statistics observed
         *
         * Outputs the start date and end date found in the observed data
         * Outputs mean, standard deviation, and total number of items
         * observed
         */
        void output_summary(std::ostream &out = std::cout);
    };
}

#endif // STAT_HPP_
