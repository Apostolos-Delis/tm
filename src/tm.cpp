
// main module for tm,

#include <iostream>
#include <vector>
#include <sqlite3.h>

#include "CLI11.hpp"
#include "tm.hpp"
#include "tag.hpp"
#include "task.hpp"


int main(int argc, char **argv) {
    CLI::App app{tm_cli::DESCRIPTION};
    app.require_subcommand(0, 1);

    bool v_flag = false;
    app.add_flag("--version,-v", v_flag, tm_cli::VERSION_FLAG_DESCRIPTION);

    auto session = app.add_subcommand("session", tm_cli::SESSION_DESCRIPTION);

    auto task = app.add_subcommand("task", tm_cli::TASK_DESCRIPTION);
    task->require_subcommand(1);
    std::string task_name;

    auto task_rm = task->add_subcommand("rm", tm_task::RM_DESCRIPTION);
    task_rm->add_option("--name,-n", task_name, tm_task::NAME_DESCRIPTION)->required();

    auto task_done = task->add_subcommand("done", tm_task::DONE_DESCRIPTION);
    task_done->add_option("--name,-n", task_name, tm_task::NAME_DESCRIPTION)->required();

    std::string due_date;
    std::string due_time = "23:59";
    std::vector<std::string> tags;
    auto task_add = task->add_subcommand("add", tm_task::ADD_DESCRIPTION);
    task_add->add_option("--name,-n", task_name, tm_task::NAME_DESCRIPTION)->required();
    task_add->add_option("--date,-d", due_date, tm_task::DATE_DESCRIPTION)->required();
    task_add->add_option("--time,-t", due_time, tm_task::TIME_DESCRIPTION);
    task_add->add_option("--tags,-l", tags, tm_task::TAGS_DESCRIPTION);

    int max_tasks;
    bool condensed = false;
    auto task_list = task->add_subcommand("list", tm_task::LIST_DESCRIPTION);
    task_list->add_option("--max,-m", max_tasks, tm_task::MAX_DESCRIPTION);
    task_list->add_flag("--condensed,-c", condensed, tm_task::CONDENSED_DESCRIPTION);

    auto tag = app.add_subcommand("tag", tm_cli::TAG_DESCRIPTION);
    tag->require_subcommand(1);
    std::string tag_name;

    bool hard_remove = false;
    auto tag_rm = tag->add_subcommand("rm", tm_tag::RM_DESCRIPTION);
    tag_rm->add_flag("--hard", hard_remove, tm_tag::HARD_RM_DESCRIPTION);
    tag_rm->add_option("--name,-n", tag_name, tm_tag::NAME_DESCRIPTION)->required();

    auto tag_add = tag->add_subcommand("add", tm_tag::ADD_DESCRIPTION);
    tag_add->add_option("--name,-n", tag_name, tm_tag::NAME_DESCRIPTION)->required();
    tag_add->add_option("--color,-c", tag_name, tm_tag::COLOR_DESCRIPTION)->required();

    int max_tags = 0;
    bool no_color = false;
    auto tag_list = tag->add_subcommand("list", tm_tag::LIST_DESCRIPTION);
    tag_list->add_option("--max,-m", max_tags, tm_tag::MAX_DESCRIPTION);
    tag_list->add_flag("--no-color", no_color, tm_tag::NOCOLOR_DESCRIPTION);

    auto stat = app.add_subcommand("stat", tm_cli::STAT_DESCRIPTION);



    //bool add_update;
    //add->add_flag("-u,--update", add_update, "Add updated files only");

    //std::vector<std::string> add_files;
    //add->add_option("files", add_files, "Files to add");

    //add->callback( [&](){
        //std::cout << "Adding:";
        //if(add_files.empty()) {
            //if(add_update)
                //std::cout << " all updated files";
            //else
                //std::cout << " all files";
        //} else {
            //for(auto file : add_files)
                //std::cout << " " << file;
        //}
    //});

    //auto commit = app.add_subcommand("commit", "Commit files");

    //std::string commit_message;
    //commit->add_option("-m,--message", commit_message, "A message")->required();

    //commit->callback( [&](){
        //std::cout << "Commit message: " << commit_message;
    //});

    CLI11_PARSE(app, argc, argv);

    if (v_flag) {
        std::cout << "tm version: " << tm_cli::VERSION << std::endl;
        return 0;
    }

    //std::cout << "\nThanks for using geet!\n" <<  std::endl;
    //sqlite3* DB;
    //int exit = 0;
    //exit = sqlite3_open("example.db", &DB);

    //if (exit) {
        //std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        //return (-1);
    //}
    //else
        //std::cout << "Opened Database Successfully!" << std::endl;
    //sqlite3_close(DB);

    return 0;
}
