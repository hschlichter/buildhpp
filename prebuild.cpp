#include "build.hpp"
#include "ninja.hpp"

auto main(int argc, char* argv[]) -> int {
    build::Graph self;
    self.addFlag("-Wall");
    self.addFlag("-std=c++20");
    self.addFlag("-DSELF_HELLO=43");

    self.addRule({
        build::Target { "build" },
        {
            build::Target { "build.cpp" }
        },
        build::Command { "clang++", "build.cpp", "-o", "build" }
    });

    build::Graph graph;

    build::Builder<Ninja> builder { "build.ninja" };
    if (!builder.build(self, build::Target { "build" }))
        return 1;
}
