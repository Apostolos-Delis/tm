
// main module for tm,

#include <iostream>
#include <vector>
#include <string>

#include "tm.hpp"


// Defines the main routine for tm, this function defines all the
// possible CLI options and subcommands for proper argument parsing
int main(int argc, char **argv) {
    CLI::App app{tm_cli::DESCRIPTION};
    app.require_subcommand(0, 1);

    bool v_flag = false;
    app.add_flag("--version,-v", v_flag, tm_cli::VERSION_FLAG_DESCRIPTION);

    auto session = app.add_subcommand("sess", tm_cli::SESSION_DESCRIPTION);
    session->require_subcommand(1);

    // Define sess start
    std::string sess_desc = "";
    int task_id;
    int sess_length;
    bool no_interupt = false;
    bool no_overtime = false;
    auto session_start = session->add_subcommand("start",
            tm_sess::START_DESCRIPTION);
    session_start->add_option("--task,-t",
            task_id, tm_sess::TASK_DESCRIPTION)->required();
    session_start->add_option("--length,-l",
            sess_length, tm_sess::LENGTH_DESCRIPTION)->required();
    session_start->add_flag("--no-interrupt,-i",
            no_interupt, tm_sess::INTERRUPT_DESCRIPTION);
    session_start->add_flag("--no-overtime,-o", no_overtime,
            tm_sess::OVERTIME_DESCRIPTION);
    session_start->add_option("--description,-d",
            sess_desc, tm_sess::DESC_DESCRIPTION);
    session_start->callback( [&]() {
            tm_sess::handle_start(sess_length, no_interupt, no_overtime,
                                  task_id, sess_desc);
    });

    // Define sess add
    std::string time_started = "00:00";
    std::string date_started;
    auto session_add = session->add_subcommand("add", tm_sess::ADD_DESCRIPTION);
    session_add->add_option("--task-id,-i",
            task_id, tm_sess::TASK_DESCRIPTION)->required();
    session_add->add_option("--length,-l",
            sess_length, tm_sess::LENGTH_DESCRIPTION)->required();
    session_add->add_option("--description,-d",
            sess_desc, tm_sess::DESC_DESCRIPTION);
    session_add->add_option("--start-date,-s", date_started,
            tm_sess::DATE_DESCRIPTION)->required();
    session_add->add_option("--time,-t", time_started, 
            tm_sess::TIME_DESCRIPTION);
    session_add->callback( [&]() {
            tm_sess::handle_add(sess_length, task_id, date_started,
                                time_started, sess_desc);
    });

    // Define sess rm
    int sess_id;
    auto session_rm = session->add_subcommand("rm", tm_sess::RM_DESCRIPTION);
    session_rm->add_option("--id,-i",
            sess_id, tm_sess::ID_DESCRIPTION)->required();
    session_rm->callback( [&]() {
            tm_sess::handle_remove(sess_id);
    });

    // Define sess log
    int max_sessions = DEFAULT_LOG_LENGTH;
    bool condensed = false;
    bool sess_log_all = false;
    bool reversed = false;
    auto session_log = session->add_subcommand("log",
            tm_sess::LOG_DESCRIPTION);
    session_log->add_option("--max,-m", max_sessions,
            tm_sess::MAX_DESCRIPTION, true);
    session_log->add_flag("--all,-a", sess_log_all,
            tm_sess::ALL_DESCRIPTION);
    session_log->add_flag("--reverse,-r", reversed,
            tm_sess::REVERSED_DESCRIPTION);
    session_log->add_flag("--condensed,-c", condensed,
            tm_sess::CONDENSED_DESCRIPTION);

    void handle_log(bool condensed, int max_sessions);
    session_log->callback( [&]() {
            if (sess_log_all) {
                max_sessions = 0;
            }
            tm_sess::handle_log(condensed, max_sessions, reversed);
    });

    // Define tm task
    auto task = app.add_subcommand("task", tm_cli::TASK_DESCRIPTION);
    task->require_subcommand(1);

    // Define task rm
    bool hard_remove = false;
    auto task_rm = task->add_subcommand("rm", tm_task::RM_DESCRIPTION);
    task_rm->add_option("--id,-i", task_id,
            tm_task::ID_DESCRIPTION)->required();
    task_rm->add_flag("--hard", hard_remove, tm_task::HARD_RM_DESCRIPTION);
    task_rm->callback( [&]() {
            tm_task::handle_rm(task_id, hard_remove);
    });
    // Define task done
    auto task_done = task->add_subcommand("done", tm_task::DONE_DESCRIPTION);
    task_done->add_option("--id,-i", task_id,
            tm_task::ID_DESCRIPTION)->required();
    task_done->add_flag("--reversed,-r", reversed,
            tm_task::REVERSED_DESCRIPTION);
    task_done->callback( [&]() {
            tm_task::handle_done(task_id, reversed);
    });

    // Define task add
    std::string task_name;
    std::string due_date;
    std::string proj_name;
    std::string due_time = "00:00";
    std::vector<std::string> tags;
    auto task_add = task->add_subcommand("add", tm_task::ADD_DESCRIPTION);
    task_add->add_option("--name,-n", task_name,
            tm_task::NAME_DESCRIPTION)->required();
    task_add->add_option("--proj,-p", proj_name, tm_task::PROJ_DESCRIPTION);
    task_add->add_option("--date,-d", due_date,
            tm_task::DATE_DESCRIPTION)->required();
    task_add->add_option("--time,-t", due_time, tm_task::TIME_DESCRIPTION);
    task_add->add_option("--tags,-l", tags, tm_task::TAGS_DESCRIPTION);
    task_add->callback( [&]() {
            tm_task::handle_add(task_name, proj_name, due_date,
                                due_time, tags);
    });

    // Define task list
    int max_tasks = 50;
    bool list_long = false;
    bool display_complete = false;
    bool display_all = false;
    std::vector<std::string> specified_tags;
    std::string date_till, date_from;
    std::string specified_date, specified_proj;
    auto task_list = task->add_subcommand("list", tm_task::LIST_DESCRIPTION);
    task_list->add_option("--max,-m", max_tasks, tm_task::MAX_DESCRIPTION);
    task_list->add_flag("--long,-l", list_long,
            tm_task::LIST_LONG_DESCRIPTION);
    task_list->add_flag("--all,-a", display_all,
            tm_task::DISPLAY_ALL_DESCRIPTION);
    task_list->add_flag("--complete,-c", display_complete,
            tm_task::DISPLAY_DONE_DESCRIPTION);
    task_list->add_flag("--reversed,-r", reversed,
            tm_task::LIST_REVERSED_DESCRIPTION);
    task_list->add_option("--date,-d", specified_date,
            tm_task::LIST_DATE_DESCRIPTION);
    task_list->add_option("--until,-u", date_till,
            tm_task::LIST_TILL_DESCRIPTION);
    task_list->add_option("--from,-f", date_from,
            tm_task::LIST_FROM_DESCRIPTION);
    task_list->add_option("--project,-p", specified_proj,
            tm_task::LIST_PROJ_DESCRIPTION);
    task_list->add_option("--tags,-t", specified_tags,
            tm_task::LIST_TAGS_DESCRIPTION);
    task_list->callback( [&]() {
            if (display_all) {
                max_tasks = 0;
            }
            if (!specified_date.empty() && 
                    (!date_from.empty() || !date_till.empty())){
                std::cerr << "ERROR: cannot set both --date and either"
                          << " --from or --after options" << std::endl;
                exit(1);
            }
            tm_task::handle_list(list_long, max_tasks,
                                 display_complete,
                                 reversed,
                                 specified_tags,
                                 specified_date,
                                 date_from, date_till,
                                 specified_proj);
    });

    // Define tm tag
    auto tag = app.add_subcommand("tag", tm_cli::TAG_DESCRIPTION);
    tag->require_subcommand(1);
    std::string tag_name;

    // Define tag rm
    auto tag_rm = tag->add_subcommand("rm", tm_tag::RM_DESCRIPTION);
    tag_rm->add_flag("--hard", hard_remove, tm_tag::HARD_RM_DESCRIPTION);
    tag_rm->add_option("--name,-n", tag_name,
            tm_tag::NAME_DESCRIPTION)->required();
    tag_rm->callback( [&]() {
            tm_tag::handle_rm(tag_name, hard_remove);
    });

    // Define tag add
    std::string color;
    auto tag_add = tag->add_subcommand("add", tm_tag::ADD_DESCRIPTION);
    tag_add->add_option("--name,-n", tag_name,
            tm_tag::NAME_DESCRIPTION)->required();
    tag_add->add_option("--color,-c", color, tm_tag::COLOR_DESCRIPTION);
    tag_add->callback( [&]() {
            tm_tag::handle_add(tag_name, color);
    });

    // Define tag list
    int max_tags = 0;
    bool no_color = false;
    auto tag_list = tag->add_subcommand("list", tm_tag::LIST_DESCRIPTION);
    tag_list->add_option("--max,-m", max_tags, tm_tag::MAX_DESCRIPTION);
    tag_list->add_flag("--no-color", no_color, tm_tag::NOCOLOR_DESCRIPTION);
    tag_list->callback( [&]() {
            tm_tag::handle_list(no_color, max_tags);
    });

    // Define tm proj
    auto proj = app.add_subcommand("proj", tm_cli::PROJ_DESCRIPTION);
    proj->require_subcommand(1);

    // Define proj rm
    bool hard = false;
    auto proj_rm = proj->add_subcommand("rm", tm_proj::RM_DESCRIPTION);
    proj_rm->add_option("--name,-n", proj_name,
            tm_proj::NAME_DESCRIPTION)->required();
    proj_rm->add_flag("--hard", hard, tm_proj::HARD_DESCRIPTION);
    proj_rm->callback( [&]() {
            tm_proj::handle_rm(proj_name, hard);
    });

    // Define proj done
    auto proj_done = proj->add_subcommand("done", tm_proj::DONE_DESCRIPTION);
    proj_done->add_option("--name,-n", proj_name,
            tm_proj::NAME_DESCRIPTION)->required();
    proj_done->add_flag("--reversed,-r", reversed,
            tm_proj::REVERSED_DESCRIPTION);
    proj_done->callback( [&]() {
            tm_proj::handle_done(proj_name, reversed);
    });

    // Define proj add
    auto proj_add = proj->add_subcommand("add", tm_proj::ADD_DESCRIPTION);
    proj_add->add_option("--name,-n", proj_name,
            tm_proj::NAME_DESCRIPTION)->required();
    proj_add->callback( [&]() {
            tm_proj::handle_add(proj_name);
    });

    // Define proj list
    bool long_format = false;
    std::vector<std::string> proj_names;
    auto proj_list = proj->add_subcommand("list", tm_proj::LIST_DESCRIPTION);
    proj_list->add_flag("--long,-l", long_format,
            tm_proj::LONG_DESCRIPTION);
    proj_list->add_flag("--complete,-c", display_complete,
            tm_proj::DISPLAY_DONE_DESCRIPTION);
    proj_list->callback( [&]() {
            tm_proj::handle_list(long_format, display_complete, proj_names);
    });

    // Define tm stat
    auto stat = app.add_subcommand("stat", tm_cli::STAT_DESCRIPTION);
    stat->require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    if (argc == 1) {
        // Print the help message if just tm is called
        std::cout << app.help() << std::flush;
    } else if (v_flag) {
        std::cout << "tm version: " << tm_cli::VERSION << std::endl;
    }
    return 0;
}
