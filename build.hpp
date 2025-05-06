#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

namespace build {
    using Path = std::filesystem::path;

    class Flags {
        public:
            auto add(std::string f) -> void { 
                flags.push_back(std::move(f));
            }

            auto empty() const -> bool { 
                return flags.empty();
            }

            auto toString() const -> std::string {
                std::ostringstream oss;
                for (size_t i = 0; i < flags.size(); ++i) {
                    if (i) { 
                        oss << ' ';
                    }

                    // no quoting here, assume user adds properly
                    oss << flags[i];
                }
                return oss.str();
            }

        private:
            std::vector<std::string> flags;
    };

    class Target {
        public:
            Target(Path p, bool phony = false) : path_(std::move(p)), phony_(phony) {}

            auto path() const -> const Path& {
                return path_;
            }

            auto isPhony() const -> bool {
                return phony_;
            }

            auto name() const -> std::string {
                return path_.string();
            }

            auto operator==(Target const& o) const -> bool {
                return path_ == o.path_ && phony_ == o.phony_;
            }

            auto operator!=(Target const& o) const -> bool {
                return !(*this == o);
            }

        private:
            Path path_;
            bool phony_;
    };

    class Command {
        public:
            Command() = default;

            template<typename... Args>
            explicit Command(Args&&... as) : args { std::forward<Args>(as)... } {}

            auto addArg(std::string a) -> void {
                args.push_back(std::move(a));
            }

            auto toString() const -> std::string {
                std::ostringstream oss;
                for (size_t i = 0; i < args.size(); ++i) {
                    auto const& a = args[i];
                    bool needQuote = a.find(' ') != std::string::npos;
                    if (needQuote) {
                        oss << '"';
                    }

                    oss << a;

                    if (needQuote) {
                        oss << '"';
                    }

                    if (i + 1 < args.size()) {
                        oss << ' ';
                    }
                }

                return oss.str();
            }

        private:
            std::vector<std::string> args;
    };

    struct Rule {
        Target target;
        std::vector<Target> deps;
        Command cmd;
    };

    class Graph {
        public:
            auto addRule(Rule r) -> void { 
                rules.emplace(r.target.name(), std::move(r));
            }

            auto addFlag(std::string f)-> void { 
                globalFlags.add(std::move(f));
            }

            std::unordered_map<std::string, Rule> rules;
            Flags globalFlags;
    };

    template <typename T>
    struct SerializerTrait {
        static auto serialize(const Graph&, const Path&) -> bool {
            static_assert(sizeof(T) == 0, "SerializerTrait not implemented for this Backend");
            return false;
        }
    };

    template <typename T>
    struct ExecutorTrait {
        static auto execute(const Graph&, const Path&, const Target&) -> bool {
            static_assert(sizeof(T) == 0, "ExecutorTrait not implemented for this Backend");
            return false;
        }
    };

    template <typename T>
    class Builder {
        public:
            explicit Builder(Path fp) : filepath(std::move(fp)) {}

            auto build(const Graph& g, const Target& desired) const -> bool {
                return ExecutorTrait<T>::execute(g, filepath, desired);
            }

        private:
            Path filepath;
    };
}

