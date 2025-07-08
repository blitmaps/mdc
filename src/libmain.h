//
// Created by Bradley Pearce on 24/03/2023.
//
#pragma once
#ifndef BLIBS_LIBMAIN_H
#define BLIBS_LIBMAIN_H

#include <vector>
#include <string>
#include <filesystem>

// simplify shared pointer
template <class T>
using pointer = std::shared_ptr<T>;


struct Arguments {
    // Preserve the standard structure
    int argc;
    char *argv;
    // Other flags
    bool help_flag;
    // Parse the arguments to a vector
    std::vector<std::string> arguments;
    std::string command;
    std::string relative_path;
};

void my_main(Arguments &args);

int main(int argc, char* argv[]) {

    auto path = std::filesystem::absolute(argv[0]).parent_path();

    Arguments args {
            .argc = argc,
            .argv = *argv,
            .help_flag = false,
            .arguments = std::vector<std::string>(argv + 1, argv + argc),
            .relative_path = path.string() + "/"
    };

    for (auto &str : args.arguments) {
        args.help_flag |= (str == "--help") || (str == "-h");
    }

    my_main(args);
    return 0;
}

#endif //BLIBS_LIBMAIN_H
