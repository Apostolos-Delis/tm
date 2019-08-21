//
// Defines the tm stat
//


#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <chrono>
#include <thread>

#include "stat.hpp"
#include "database.hpp"
#include "utils.hpp"


#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })


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
    std::string query_from, query_until;

    // Check year
    if (!year.empty() && tm_utils::valid_date(year + "-01-01")) {
        query_from = year + "-01-01";
        query_until = year + "-12-31";
    } else if (!year.empty()) {
        std::cerr << "ERROR: '" << year << "' is not a valid year!" << std::endl;
        exit(1);
    }

    // Check from
    if (!from.empty() && tm_utils::valid_date(from)) {
        query_from = from;
    } else if (!from.empty()) {
        std::cerr << "ERROR: '" << from << "' is not a valid date!" << std::endl;
        exit(1);
    }

    // Check from
    if (!until.empty() && tm_utils::valid_date(until)) {
        query_until = until;
    } else if (!until.empty()) {
        std::cerr << "ERROR: '" << until << "' is not a valid date!" << std::endl;
        exit(1);
    }

    // If sum_all, no date restrictions added
    if (sum_all) {
        query_until = "";
        query_from = "";
    }

    // Get Total
    auto db = tm_db::TMDatabase();

    std::unordered_map<std::string, double> data;
    if (sum_tasks) {
        data = db.stat_task_query(query_from, query_until);
    } else {
        data = db.stat_time_query(query_from, query_until);
    }
    auto stat = StatHandler(data);
    stat.output_summary();
}


void tm_stat::handle_grad() {
    auto db = tm_db::TMDatabase();
    auto data = db.stat_time_query("", "");
    auto stat = StatHandler(data);
    stat.gradient_map();
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
inline void tm_stat::StatHandler::load_vals() {
    for (auto const& it : this->data_) {
        if (this->min_date.empty() || this->min_date > it.first) {
            this->min_date = it.first;
        }
        if (this->max_date.empty() || this->max_date < it.first) {
            this->max_date = it.first;
        }
        this->vals_.push_back(it.second);
    }
}


/**
 * Description: Convert date to Rata Die
 * Reference: https://en.wikipedia.org/wiki/Rata_Die
 * @param[in] date: the date in 'YYYY-MM-DD' Format
 * @return Returns an int representing the date
 */
int rdn(const std::string &date) {
    if (!tm_utils::valid_date(date)) {
        std::cerr << "Error: '" << date << "' is not a valid date!" << std::endl;
        exit(1);
    }
    int y = stoi(date.substr(0, 4));
    int m = stoi(date.substr(5, 2));
    int d = stoi(date.substr(8, 2));
    if (m < 3) {
        y--;
        m += 12;
    }
    return 365*y + y/4 - y/100 + y/400 + (153*m - 457)/5 + d - 306;
}

/**
 * Description: Returns the number of days between date1 and date2
 * date1 and date2 both need to be valid dates of the form YYYY-MM-DD
 */
inline int day_diff(const std::string &date1, const std::string &date2) {
    return rdn(date2) - rdn(date1);
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

    auto mean = tm_math::mean(this->vals_);
    auto stdev = tm_math::standard_dev(this->vals_, mean);
    auto total_days = day_diff(this->min_date, this->max_date);
    auto total_mean = (mean * this->vals_.size()) / total_days;

    out << "\033[1;4;39mDisplaying Statistical Summary\033[0m" << std::endl;
    out << "Observing data from:" << std::endl;
    out << "Earliest datapoint: '" << this->min_date << "'\n";
    out << "Latest datapoint: '" << this->max_date << "'\n\n";
    out << "Total days in between: " << total_days << std::endl;
    out << "Average value for days this->data_ed: " << std::setw(2) << mean << std::endl;
    out << "Standard deviation: " << std::setw(2) << stdev << std::endl;
    out << "Average value for all days: " << std::setw(2) << total_mean << std::endl;
    out << "Total number of datapoints observed: " << this->vals_.size()
        << std::endl;
}


/**
 * Description: Returns the current day of the week, 0 is Monday, 6 is Sunday
 */
inline int cur_day_of_week() {
    time_t rawtime;
    tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo->tm_wday;
}


/**
 * Description: Operator overload to allow for chrono timestamps
 * to be converted to strings
 * @param[in] os: the output stream to send the output to
 * @param[in] timep: the time_point as calculated from convert_to_timepoint
 * @return Returns
 */
template <typename Clock, typename Duration>
std::ostream& operator<<(std::ostream& os,
        const std::chrono::time_point<Clock, Duration>& timep) {

    auto converted_timep = Clock::to_time_t(timep);
    os << std::put_time(std::localtime(&converted_timep), "%Y-%m-%d");
    return os;
}

/**
 * Description: Returns a new time_point based on how many days to add,
 * note that if days_to_add is a negative number, then it will subtract days
 *
 * @param[in] timepoint: a time_point object
 * @param[in] days_to_add: The number of days to add
 * @return Returns a new time_point shifted by the specified amount of days
 */
template <typename Clock, typename Duration>
auto add_days(const std::chrono::time_point<Clock, Duration>& timepoint,
              int days_to_add) {
    constexpr std::time_t seconds_in_day = 60 * 60 * 24;
    //                                     mm   hh   dd

    std::time_t days = seconds_in_day * days_to_add;
    auto date = Clock::to_time_t(timepoint);
    return Clock::from_time_t(date + days);
}


/**
 * Description: converts 3 integers into a time_point object, which
 * allows for easy manipulation across datetime
 * @param[in] years: integer that is greater than 1900 ideally
 * @param[in] months: a value from 1 - 12
 * @param[in] days: a value from 1 - 31
 * @return Returns a time_point object
 */
auto convert_to_timepoint(int years, int months, int days) {
    --months; // Months needs to be from 0-11 not 1-12
    years -= 1900;
    std::tm date = {};
    date.tm_year = years;
    date.tm_mon = months;
    date.tm_mday = days;
    return std::chrono::system_clock::from_time_t(std::mktime(&date));
}


/**
* Description: Prints a gradient map of your progress, in a similar style to
* Github with its contribution chart
* @param[in] color: the color of the gradient, must be a valid color from the
* tm_color::VALID_GRADIENTS set
*/
void tm_stat::StatHandler::gradient_map(std::string color, std::ostream &out) {

    std::string now = tm_utils::current_datetime().substr(0, 10);

    int year = stoi(now.substr(0, 4));
    int month = stoi(now.substr(5, 2));
    int day = stoi(now.substr(8, 2));

    std::cout << "Current Day: " << now << std::endl;

    auto date = convert_to_timepoint(year, month, day);

    double min_val = 0xfffff;
    double max_val = -1;
    for (auto const& it : this->data_) {
        if (it.second <= min_val) {
            min_val = it.second;
        }
        if (it.second >= max_val) {
            max_val = it.second;
        }
    }

    //Normalize the values between 0 and 1
    for (auto it = this->data_.begin(); it != this->data_.end(); it++) {
        it->second = tm_math::normalize(it->second, min_val, max_val);
    }

    ssize_t grad_size = tm_color::GRADIENTS.find(color)->second.size();

    double interval = 1.0 / grad_size;

    std::vector<std::string> progress;
    constexpr int full_weeks = 52 * 7;
    for (int i = full_weeks + cur_day_of_week(); i >= 0; --i) {
        std::stringstream ss;
        ss << add_days(date, -i);
        std::string iter_date = ss.str();
        auto it = this->data_.find(iter_date);
        if (it == this->data_.end()) {
            progress.push_back("▦ ");
        } else {
            int score = MIN(static_cast<int>(this->data_[iter_date] / interval),
                            grad_size - 1);

            std::stringstream ss_format;
            auto temp_val = tm_color::GRADIENTS.find(color)->second[score];
            ss_format << "\033[38;5;" << temp_val << "m"
                      << "▦ " << "\033[0m";
            progress.push_back(ss_format.str());
        }
    }

    const std::vector<std::string> DAYS = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const std::vector<std::string> MONTHS = {"Jan ", "Feb ", "Mar ", "Apr ", "May ", "Jun ",
                                             "Jul ", "Aug ", "Sep ", "Oct ", "Nov ", "Dec"};

    std::cout << "Less ";
    for (int i = 0; i < grad_size; ++i) {
        auto temp_val = tm_color::GRADIENTS.find(color)->second[i];
            std::cout << "\033[38;5;" << temp_val << "m▦ " << "\033[0m";
    }
    std::cout << "More" << std::endl;

    // Print Month bar
    std::cout << "Month: ";
    int cur_month = month;
    int num_months = 0;
    std::string month_out;
    for (int i = progress.size() / 7; i >= 0; --i) {
        std::stringstream ss;
        ss << add_days(date, i * -7);
        month = stoi(ss.str().substr(5, 2));
        day = stoi(ss.str().substr(8, 2));
        if (cur_month != month && !(cur_month == 0 && month == 12)) {
            month_out = MONTHS[month - 1];
            std::cout << month_out;
            cur_month = month % 12;
            num_months++;
        }
        else if (day > 13 && num_months != 12) {
            std::cout << "  ";
        }
    }
    std::cout << std::endl;

    for (int j = 0; j < 7; ++j) {
        std::cout << DAYS[j] << ":   ";
        for (int i = j; i < progress.size(); i+= 7) {
            std::cout << progress[i];
        }
        std::cout << "\033[0m" << std::endl;
    }
}
