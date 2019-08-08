//
// Defines the tm stat
//


#include <string>
#include <iostream>

#include "stat.hpp"
#include "database.hpp"

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
void tm_stat::handle_summary(bool sum_tasks, bool sum_all,
                             const std::string &year,
                             const std::string &from,
                             const std::string &until) {
    auto db = tm_db::TMDatabase();
}
