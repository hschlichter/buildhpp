#include <cstdlib>
#include <iostream>
#include "build.hpp"
#include "ninja.hpp"

auto generate_prebuild() -> int {
    build::Graph prebuild;
    prebuild.addFlag("-Wall");
    prebuild.addFlag("-std=c++20");

    prebuild.addRule({
        build::Target { "prebuild" },
        {
            build::Target { "prebuild.cpp" }
        },
        build::Command { "clang++", "prebuild.cpp", "-o", "prebuild" }
    });

    build::Builder<Ninja> builder { "prebuild.ninja" };
    if (!builder.build(prebuild, build::Target { "prebuild" }))
        return 1;

    return 0;
}

auto main(int argc, char* argv[]) -> int {
    // 0. Gather arguments for build step
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <target>\n";
        return 1;
    }

    // 1. Build ./prebuild
    int ret = generate_prebuild();
    if (ret != 0) {
        std::cerr << "Error: generate_prebuild failed (exit " << ret << ")\n";
        return 1;
    }

    // 2. Run ./prebuild to build ./build
    ret = std::system("./prebuild");
    if (ret != 0) {
        std::cerr << "Error: prebuild build failed (exit " << ret << ")\n";
        return 1;
    }

    // 3. Run ./build
    std::string buildCmd = "./build";
    buildCmd.append(" ");
    buildCmd.append(argv[1]);
    ret = std::system(buildCmd.c_str());
    if (ret != 0) {
        std::cerr << "Error: build build failed (exit " << ret << ")\n";
        return 1;
    }


    return 0;
}
