
// main module for tm,

#include <iostream>
#include <sqlite3.h>

#include "parser.hpp"
#include "CLI11.hpp"


int main(int argc, char **argv) {
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open("example.db", &DB);

    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        return (-1);
    }
    else
        std::cout << "Opened Database Successfully!" << std::endl;
    sqlite3_close(DB);
    //auto ap = parser::ArgParse(argc, argv);
    CLI::App app{"Geet, a command line git lookalike that does nothing"};
    app.require_subcommand(1);

    auto add = app.add_subcommand("add", "Add file(s)");

    bool add_update;
    add->add_flag("-u,--update", add_update, "Add updated files only");

    std::vector<std::string> add_files;
    add->add_option("files", add_files, "Files to add");

    add->callback( [&](){
        std::cout << "Adding:";
        if(add_files.empty()) {
            if(add_update)
                std::cout << " all updated files";
            else
                std::cout << " all files";
        } else {
            for(auto file : add_files)
                std::cout << " " << file;
        }
    });

    auto commit = app.add_subcommand("commit", "Commit files");

    std::string commit_message;
    commit->add_option("-m,--message", commit_message, "A message")->required();

    commit->callback( [&](){
        std::cout << "Commit message: " << commit_message;
    });

    CLI11_PARSE(app, argc, argv);

    std::cout << "\nThanks for using geet!\n" <<  std::endl;

    return 0;
}
