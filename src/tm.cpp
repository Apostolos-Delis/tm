
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
    std::string sess_task;
    std::string sess_desc = "";
    int sess_length;
    bool no_interupt = false;
    bool no_overtime = false;
    auto session_start = session->add_subcommand("start",
            tm_sess::START_DESCRIPTION);
    session_start->add_option("--task,-t",
            sess_task, tm_sess::TASK_DESCRIPTION)->required();
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
                                  sess_task, sess_desc);
    });

    // Define sess log
    int max_sessions = DEFAULT_LOG_LENGTH;
    bool condensed = false;
    auto session_log = session->add_subcommand("log",
            tm_sess::LOG_DESCRIPTION);
    session_log->add_option("--max,-m", max_sessions,
            tm_sess::MAX_DESCRIPTION, true);
    session_log->add_flag("--condensed,-c", condensed,
            tm_sess::CONDENSED_DESCRIPTION);

    void handle_log(bool condensed, int max_sessions);
    session_log->callback( [&]() {
            tm_sess::handle_log(condensed, max_sessions);
    });

    // Define tm task
    auto task = app.add_subcommand("task", tm_cli::TASK_DESCRIPTION);
    task->require_subcommand(1);

    // Define task rm
    int task_id;
    auto task_rm = task->add_subcommand("rm", tm_task::RM_DESCRIPTION);
    task_rm->add_option("--id,-i", task_id,
            tm_task::ID_DESCRIPTION)->required();
    task_rm->callback( [&]() {
            tm_task::handle_rm(task_id);
    });
    // Define task done
    auto task_done = task->add_subcommand("done", tm_task::DONE_DESCRIPTION);
    task_done->add_option("--id,-i", task_id,
            tm_task::ID_DESCRIPTION)->required();
    task_done->callback( [&]() {
            tm_task::handle_done(task_id);
    });

    // Define task add
    std::string task_name;
    std::string due_date;
    std::string due_time = "23:59";
    std::vector<std::string> tags;
    auto task_add = task->add_subcommand("add", tm_task::ADD_DESCRIPTION);
    task_add->add_option("--name,-n", task_name,
            tm_task::NAME_DESCRIPTION)->required();
    task_add->add_option("--date,-d", due_date,
            tm_task::DATE_DESCRIPTION)->required();
    task_add->add_option("--time,-t", due_time, tm_task::TIME_DESCRIPTION);
    task_add->add_option("--tags,-l", tags, tm_task::TAGS_DESCRIPTION);
    task_add->callback( [&]() {
            tm_task::handle_add(task_name, due_date, due_time, tags);
    });

    // Define task list
    int max_tasks;
    bool display_complete = false;
    std::vector<std::string> specified_tags;
    std::string specified_date;
    auto task_list = task->add_subcommand("list", tm_task::LIST_DESCRIPTION);
    task_list->add_option("--max,-m", max_tasks, tm_task::MAX_DESCRIPTION);
    task_list->add_flag("--condensed,-c", condensed,
            tm_task::CONDENSED_DESCRIPTION);
    task_list->add_flag("--display-complete,-f", display_complete,
            tm_task::DISPLAY_DONE_DESCRIPTION);
    task_list->add_option("--date,-d", specified_date, 
            tm_task::LIST_DATE_DESCRIPTION);
    task_list->add_option("--tags,-t", specified_tags,
            tm_task::LIST_TAGS_DESCRIPTION);
    task_list->callback( [&]() {
            tm_task::handle_list(condensed, max_tasks,
                                 display_complete,
                                 specified_tags,
                                 specified_date);
    });

    // Define tm tag
    auto tag = app.add_subcommand("tag", tm_cli::TAG_DESCRIPTION);
    tag->require_subcommand(1);
    std::string tag_name;

    // Define tag rm
    bool hard_remove = false;
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
    std::string proj_name;
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
    proj_done->callback( [&]() {
            tm_proj::handle_done(proj_name);
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
    void handle_list(bool show_tasks, bool display_done,
                     const std::vector<std::string> &proj_names);
    auto proj_list = proj->add_subcommand("list", tm_proj::LIST_DESCRIPTION);
    proj_list->add_flag("--long,-l", long_format,
            tm_proj::LONG_DESCRIPTION);
    proj_list->add_flag("--display-complete,-f", display_complete,
            tm_proj::DISPLAY_DONE_DESCRIPTION);
    proj_list->callback( [&]() {
            tm_proj::handle_list(long_format, display_complete, proj_names);
    });

    //auto stat = app.add_subcommand("stat", tm_cli::STAT_DESCRIPTION);

    CLI11_PARSE(app, argc, argv);

    if (argc == 1) {
        // Print the help message if just tm is called
        std::cout << app.help() << std::flush;
    } else if (v_flag) {
        std::cout << "tm version: " << tm_cli::VERSION << std::endl;
    }
    return 0;
}
