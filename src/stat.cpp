//
// Defines the tm stat
//


#include <string>
#include <iostream>
#include <iomanip>

#include "stat.hpp"
#include "database.hpp"
#include "utils.hpp"

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

    const constexpr int DAY_IN_SECS = 60 * 60 * 24;

    //std::string query_from, query_until;
    //if (!year.empty()) {

    //}

    // Get Total
    auto db = tm_db::TMDatabase();
}


/**
 * @param[in] data: a map that maps dates -> values
 */
tm_stat::StatHandler::StatHandler(const std::unordered_map<
                                  std::string, double> &data) {
    this->data_ = data;
    this->load_vals();
}


/**
 * Description: loads all the values from data_ into vals_
 */
void tm_stat::StatHandler::load_vals() {
    for (auto const& it : this->data_) {
        if (this->min_date.empty() || this->min_date > it.first) {
            this->min_date = it.first;
        }
        if (this->max_date.empty() || this->max_date > it.first) {
            this->max_date = it.first;
        }
        this->vals_.push_back(it.second);
    }
}


/**
 * Description: Outputs a summary about the statistics observed
 *
 * Outputs the start date and end date found in the observed data
 * Outputs mean, standard deviation, and total number of items
 * observed
 */
void tm_stat::StatHandler::output_summary(std::ostream &out) {
    if (this->vals_.empty()) { return; }

    auto mean = tm_math::mean<double>(this->vals_);
    auto stdev = tm_math::standard_dev<double>(this->vals_, mean);

    out << "Observing data from:" << std::endl;
    out << this->min_date << " to " << this->max_date << std::endl;
    out << "Average Value: " << std::setw(2) << mean << std::endl;
    out << "Standard Deviation: " << std::setw(2) << stdev << std::endl;
    out << "Total Number of Values Observed: " << this->vals_.size()
        << std::endl;
}
