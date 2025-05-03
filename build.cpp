#include <iostream>
#include "build.hpp"
#include "ninja.hpp"

// This program will be built by self
auto main(int argc, char* argv[]) -> int {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <target>\n";
        return 1;
    }

    build::Target desired{ argv[1] };

    build::Graph graph;
    graph.addFlag("-Wall");
    graph.addFlag("-std=c++20");
    graph.addFlag("-DHELLO");
    graph.addFlag("-DWORLD");
    graph.addFlag("-DFUBAR=43");
    graph.addFlag("-DFUBAR2=hello");

    graph.addRule({ 
        build::Target { "simple.o" }, 
        { 
            build::Target { "simple.cpp" } 
        }, 
        build::Command { "clang++", "-c", "simple.cpp", "-o", "simple.o" } 
    });

    graph.addRule({ 
        build::Target { "app" }, 
        {
            build::Target { "simple.o" }
        }, 
        build::Command { "clang++", "simple.o", "-o", "app" } 
    });

    build::Builder<Ninja> builder { "app.ninja" };
    if (!builder.build(graph, desired))
        return 1;

    return 0;
}

