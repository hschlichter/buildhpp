#include "build.hpp"

#include <fstream>
#include <iostream>
#include <cstdlib>

struct Ninja;

template <>
struct build::SerializerTrait<Ninja> {
    static auto serialize(const Graph& g, const Path& file) -> bool {
        std::ofstream out(file);
        if (!out.is_open()) {
            std::cerr << "Error: cannot open " << file << "\n";
            return false;
        }

        std::string flags;
        if (!g.globalFlags.empty()) {
            flags = g.globalFlags.toString() + " ";
        }

        out
            << "ninja_required_version = 1.3\n\n"
            "flags = " << flags << "\n\n"
            "rule cmd\n"
            "  command = $cmd $flags\n"
            "  description = building $out\n\n";

        for (auto const& kv : g.rules) {
            auto const& r = kv.second;
            out << "build " << r.target.name() << (r.target.isPhony() ? ": phony " : ": ") << "cmd";
            for (auto const& d : r.deps) {
                out << " " << d.name();
            }

            out << "\n  cmd = "
                << r.cmd.toString()
                << "\n\n";
        }

        return true;
    }
};

template <>
struct build::ExecutorTrait<Ninja> {
    static auto execute(const Graph& g, const Path& file, const Target& desired) -> bool {
        if (!SerializerTrait<Ninja>::serialize(g, file))
            return false;

        std::cout << "Generated " << file << "\n";
        std::string cmd = "ninja -v -f " + file.string() + " " + desired.name();
        std::cout << "Running: " << cmd << "\n";

        int rc = std::system(cmd.c_str());
        if (rc != 0) {
            std::cerr << "Error: ninja failed (exit " << rc << ")\n";
            return false;
        }

        return true;
    }
};


