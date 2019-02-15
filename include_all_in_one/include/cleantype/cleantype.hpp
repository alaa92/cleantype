#pragma once

// CleanType : amalgamated version
//
// This file is part of CleanType: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

//////////////////////////////////////////
////   Header: cleantype.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

//////////////////////////////////////////
////   Header: cleantype_configuration.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

#include <array>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// In order to configure the behavior of clean types :
//
// * Duplicate the file .cleantype.json at the root of the cleantype repository,
//   and save this file in anywhere in the hierarchy of the parent directories
//   of the execution directory.
// * Edit this file as a normal json file (you can use also use an online editor like
//   https://jsoneditoronline.org/)
// * #define CLEANTYPE_USE_NLOHMANN_JSON before including this file and
//   add the include path to nlohmann/json.hpp)

namespace cleantype
{
    struct CleanConfiguration;
    namespace internal
    {
        CleanConfiguration ImplGlobalConfig();
    }  // namespace internal

    struct CleanConfiguration
    {
        std::vector<std::string> suppress_extra_namespaces_ = {"::__1", "::__cxx11"};

        // unnecessary class, struct given by msvc
        std::vector<std::string> suppress_extract_struct_class_ = {"class ", "struct "};

        // add your custom suppressions here
        std::vector<std::string> suppress_custom_ = {" __ptr64"};

        // these node are suppressed after parse of the template tree of a type
        std::vector<std::string> undesirable_type_nodes_ = {
            "std::char_traits", "std::allocator", "std::less"};

        std::map<std::string, std::string> replacements_after_undesirable_node_extractions = {
            {"std::basic_string<char>", "std::string"},
            {"basic_string<_CharT, _Traits, _Allocator>", "std::string"}};

        // set this to true in order to force the results to be east const
        // this is experimental, and can fail
        bool force_east_const_ = false;

        // * any type with a depth <= indent_depth_limit
        //   will be presented on a single line
        // * any type with a depth > indent_depth_limit
        //   will be presented on several indented lines
        // set  indent_depth_limit = 0 to disable indentation
        std::size_t indent_depth_limit = 3;

        inline static CleanConfiguration & GlobalConfig()
        {
            thread_local CleanConfiguration gCleanConfiguration = internal::ImplGlobalConfig();
            return gCleanConfiguration;
        }

        inline static CleanConfiguration _EmptyConfig()
        {
            CleanConfiguration r;
            r.suppress_extra_namespaces_ = {};
            r.suppress_extract_struct_class_ = {};
            r.suppress_custom_ = {};
            r.undesirable_type_nodes_ = {};
            r.replacements_after_undesirable_node_extractions = {};
            r.force_east_const_ = false;
            r.indent_depth_limit = 0;
            return r;
        }
    };

    inline bool force_east_const() { return CleanConfiguration::GlobalConfig().force_east_const_; }
    inline void set_force_east_const(bool v)
    {
        CleanConfiguration::GlobalConfig().force_east_const_ = v;
    }

    inline std::size_t indent_depth_limit()
    {
        return CleanConfiguration::GlobalConfig().indent_depth_limit;
    }
    inline void set_indent_depth_limit(std::size_t v)
    {
        CleanConfiguration::GlobalConfig().indent_depth_limit = v;
    }

}  // namespace cleantype


//////////////////////////////////////////
////   Header: details/cleantype_configuration_json.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)


//////////////////////////////////////////
////   Header: details/filesystem.hpp
//////////////////////////////////////////
// filesystem polyfills for C++14 (70's style)

//////////////////////////////////////////
////   Header: details/cleantype_fp/fp_base.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

// These functions are inspired by (FunctionalPlus)[https://github.com/Dobiasd/FunctionalPlus],
// a Functional Programming Library for C++, which I highly recommend.
// See fplus its api search site (a la Haskell): http://www.editgym.com/fplus-api-search/
// and see demo at
// https://code-ballads.net/generated-notebooks/cpp/repl_cling/markdown/#A-REPL-session-of-C++-functional-programming,-using-fplus

// Note: These versions are intentionally simplified and not completely generic.


#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace cleantype_fp
{
    inline std::string repeat(std::size_t n, const std::string & xs)
    {
        std::string xss;
        for (std::size_t i = 0; i < n; i++)
            xss += xs;
        return xss;
    }

    template <typename F, typename T, typename U = T>
    std::vector<U> transform(F f, const std::vector<T> & xs)
    {
        std::vector<U> out;
        for (const auto & x : xs)
            out.push_back(f(x));
        return out;
    }

    template <typename T, typename F>
    std::vector<T> keep_if(F f, const std::vector<T> & xs)
    {
        std::vector<T> out;
        for (const auto & v : xs)
        {
            if (f(v))
                out.push_back(v);
        }
        return out;
    }

    inline std::string join(std::string const & separator, const std::vector<std::string> & xs)
    {
        std::string out;
        for (size_t idx = 0; idx < xs.size(); idx++)
        {
            out += xs[idx];
            if (idx < xs.size() - 1)
                out += separator;
        }
        return out;
    }

    inline std::string join(std::string const & separator, const std::deque<std::string> & xs)
    {
        std::string out;
        for (size_t idx = 0; idx < xs.size(); idx++)
        {
            out += xs[idx];
            if (idx < xs.size() - 1)
                out += separator;
        }
        return out;
    }

    template <typename T>
    std::vector<T> trim(const T & trim_what, const std::vector<T> & xs)
    {
        bool stop = false;
        std::vector<T> out;
        for (const auto & v : xs)
        {
            if (stop)
                out.push_back(v);
            else if (v != trim_what)
            {
                out.push_back(v);
                stop = true;
            }
        }
        while ((!out.empty()) && (out.back() == trim_what))
            out.pop_back();
        return out;
    }

    inline std::string trim(const char & trim_what, std::string const & xs)
    {
        bool stop = false;
        std::string out;
        for (const auto & v : xs)
        {
            if (stop)
                out.push_back(v);
            else if (v != trim_what)
            {
                out.push_back(v);
                stop = true;
            }
        }
        while ((!out.empty()) && (out.back() == trim_what))
            out.pop_back();
        return out;
    }

    inline std::string trim_whitespace(std::string const & s)
    {
        std::string r = trim(' ', s);
        r = trim('\t', r);
        r = trim('\n', r);
        return r;
    }

    inline std::string replace_tokens(const std::string & from,
                                      const std::string & to,
                                      const std::string & xs)
    {
        if (from.empty())
            return xs;
        std::string out = xs;
        size_t start_pos = 0;
        while ((start_pos = out.find(from, start_pos)) != std::string::npos)
        {
            out.replace(start_pos, from.length(), to);
            start_pos += to.length();  // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return out;
    }

    template <typename T, typename Pred>
    std::vector<T> keep_if(Pred pred, std::vector<T> && xs)
    {
        std::vector<T> r;
        for (const auto & v : xs)
            if (pred(v))
                r.push_back(v);
        return r;
    }

    template <typename T>
    std::vector<T> numbers(const T start, const T end)
    {
        std::vector<T> r;
        T v = start;
        while (v < end)
            r.push_back(v++);
        return r;
    }

    template <typename T>
    std::vector<T> numbers(const T end)
    {
        return numbers<T>(static_cast<T>(0), end);
    }

    template <typename T, typename U>
    std::vector<std::pair<T, U>> zip(const std::vector<T> & xs, const std::vector<U> & ys)
    {
        std::vector<std::pair<T, U>> r;

        auto len = std::min(xs.size(), ys.size());
        for (std::size_t idx = 0; idx < len; idx++)
        {
            r.push_back({xs[idx], ys[idx]});
        }
        return r;
    }

    template <typename T, typename U>
    std::pair<std::vector<T>, std::vector<U>> unzip(const std::vector<std::pair<T, U>> & xs)
    {
        std::pair<std::vector<T>, std::vector<U>> r;
        for (const auto & v : xs)
        {
            r.first.push_back(v.first);
            r.second.push_back(v.second);
        }
        return r;
    }

    inline std::string to_upper_case(const std::string & xs)
    {
        std::string r = xs;
        std::transform(r.begin(), r.end(), r.begin(), ::toupper);
        return r;
    }

    inline std::string to_lower_case(const std::string & xs)
    {
        std::string r = xs;
        std::transform(r.begin(), r.end(), r.begin(), ::tolower);
        return r;
    }

    template <typename T>
    T maximum(const std::vector<T> & xs)
    {
        assert(!xs.empty());
        T max = xs[0];
        for (const auto & v : xs)
            if (v > max)
                max = v;
        return max;
    }

    template <typename T>
    std::vector<T> reverse(const std::vector<T> & xs)
    {
        std::vector<T> r;
        for (std::size_t i = xs.size(); i > 0; i--)
            r.push_back(xs[i - 1]);
        return r;
    }

}  // namespace cleantype_fp


//////////////////////////////////////////
////   Header: details/stringutils.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)


#include <cassert>
#include <string>
#include <vector>

namespace cleantype
{
    namespace stringutils
    {
        inline std::string remove_spaces_before(const char token, std::string const & str)
        {
            std::string result;
            bool space_before = false;
            for (auto c : str)
            {
                if ((c == token) && space_before)
                {
                    result.pop_back();
                }
                result = result + c;

                if (c == ' ')
                    space_before = true;
                else
                    space_before = false;
            }
            return result;
        }

        inline std::string remove_spaces_after(const char token, std::string const & str)
        {
            std::string result;
            bool token_before = false;
            for (auto c : str)
            {
                result = result + c;
                if ((c == ' ') && token_before)
                    result.pop_back();

                if (c == token)
                    token_before = true;
                else
                    token_before = false;
            }
            return result;
        }

        inline std::string remove_spaces_before_after(const char token, std::string const & str)
        {
            std::string result;
            result = remove_spaces_before(token, remove_spaces_after(token, str));
            return result;
        }

        inline std::string insert_spaces_before(const char token, std::string const & str)
        {
            std::string result;
            bool space_or_same_token_before = true;
            for (auto c : str)
            {
                if ((c == token) && !(space_or_same_token_before))
                    result = result + " ";
                result = result + c;
                if ((c == ' ') || (c == token))
                    space_or_same_token_before = true;
                else
                    space_or_same_token_before = false;
            }
            return result;
        }

        inline std::string insert_spaces_after(const char token, std::string const & str)
        {
            std::string result;
            bool token_before = false;
            for (auto c : str)
            {
                if (token_before && (c != ' '))
                    result = result + ' ';
                result = result + c;
                token_before = (c == token);
            }
            return result;
        }

        inline std::string insert_spaces_before_after(const char token, std::string const & str)
        {
            std::string result = insert_spaces_before(token, str);
            result = insert_spaces_after(token, result);
            return result;
        }

        inline std::string replace_tokens(std::string const & from,
                                          const std::string & to,
                                          std::string const & xs)
        {
            if (from.empty())
                return xs;
            std::string out = xs;
            std::size_t start_pos = 0;
            while ((start_pos = out.find(from, start_pos)) != std::string::npos)
            {
                out.replace(start_pos, from.length(), to);
                start_pos +=
                    to.length();  // In case 'to' contains 'from', like replacing 'x' with 'yx'
            }
            return out;
        }

        inline bool ends_with(std::string const & value, std::string const & ending)
        {
            if (ending.size() > value.size())
                return false;
            return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
        }

        inline bool starts_with(std::string const & value, std::string const & start)
        {
            return (value.find(start) == 0);
        }

        inline bool starts_ends_with(std::string const & value,
                                     std::string const & start,
                                     std::string const & end)
        {
            return (starts_with(value, start) && ends_with(value, end));
        }

        inline std::string remove_end(std::string const & value, std::string const & ending)
        {
            assert(ends_with(value, ending));
            std::string r = value.substr(0, value.size() - ending.size());
            return r;
        }

        inline std::string remove_start(std::string const & value, std::string const & start)
        {
            assert(starts_with(value, start));
            std::string r = value.substr(start.size(), value.size() - start.size());
            return r;
        }

        inline std::string remove_start_end(std::string const & value,
                                            std::string const & start,
                                            std::string const & end)
        {
            std::string r = remove_start(value, start);
            r = remove_end(r, end);
            return r;
        }

        inline std::vector<std::string> split_string(std::string const & s, char delimiter)
        {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter))
                tokens.push_back(token);
            return tokens;
        }

    }  // namespace stringutils
}  // namespace cleantype

#include <cstdlib>
#include <fstream>
#include <sys/stat.h>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/types.h>
#include <unistd.h>
#endif
#if defined(__unix__) || defined(__APPLE__)
#include <sys/param.h>
#endif
#ifdef _WIN32
#include <direct.h>
#endif

namespace cleantype
{
    namespace filesystem
    {
        inline std::string getcwd()
        {
#ifdef _WIN32
            char * answer = _getcwd(NULL, 0);
#else
#ifdef __APPLE__
            char buffer[PATH_MAX];
#else
            char buffer[MAXPATHLEN];
#endif
            char * answer = ::getcwd(buffer, sizeof(buffer));
#endif

            std::string s;
            if (answer)
                s = answer;
            return s;
        }

        inline std::vector<std::string> parent_directories()
        {
            std::string cwd = cleantype::filesystem::getcwd();
            cwd = cleantype_fp::replace_tokens(std::string("\\"), std::string("/"), cwd);

            std::vector<std::string> folder_elems = stringutils::split_string(cwd, '/');

            if (folder_elems.empty())
                return {};

            std::vector<std::string> parent_dirs;
            std::string current = "";
            for (const auto & folder_elem : folder_elems)
            {
                current = current + folder_elem + "/";
                parent_dirs.push_back(current);
            }

            parent_dirs = cleantype_fp::reverse(parent_dirs);
            return parent_dirs;
        }

        inline bool file_exists(const std::string & filename)
        {
            struct stat buffer;
            return (stat(filename.c_str(), &buffer) == 0);
        }

        inline std::string read_istream(std::istream & is)
        {
            std::stringstream ss;
            std::string line;
            while (!is.eof())
            {
                std::getline(is, line);
                ss << line << "\n";
            }
            return ss.str();
        }

        inline std::string read_file(const std::string & filename)
        {
            std::ifstream is(filename);
            return read_istream(is);
        }
    }  // namespace filesystem
}

#ifndef CLEANTYPE_USE_NLOHMANN_JSON
namespace cleantype {
  namespace internal {
      inline CleanConfiguration ImplGlobalConfig() { return cleantype::CleanConfiguration(); }
  }
}
#else

#include <array>
#include <fstream>
#include <map>
#include <string>
#include <vector>


#include <nlohmann/json.hpp>

namespace cleantype
{

    inline void to_json(nlohmann::json & j, const cleantype::CleanConfiguration & c)
    {
        j = nlohmann::json{{"suppress_extra_namespaces_", c.suppress_extra_namespaces_},
                           {"suppress_extract_struct_class_", c.suppress_extract_struct_class_},
                           {"suppress_custom_", c.suppress_custom_},
                           {"undesirable_type_nodes_", c.undesirable_type_nodes_},
                           {"replacements_after_undesirable_node_extractions",
                            c.replacements_after_undesirable_node_extractions},
                           {"force_east_const_", c.force_east_const_},
                           {"indent_depth_limit", c.indent_depth_limit}};
    }

    inline void from_json(const nlohmann::json & j, cleantype::CleanConfiguration & c)
    {
        c = cleantype::CleanConfiguration::_EmptyConfig();
        j.at("suppress_extra_namespaces_").get_to(c.suppress_extra_namespaces_);
        j.at("suppress_extract_struct_class_").get_to(c.suppress_extract_struct_class_);
        j.at("suppress_custom_").get_to(c.suppress_custom_);
        j.at("undesirable_type_nodes_").get_to(c.undesirable_type_nodes_);
        j.at("replacements_after_undesirable_node_extractions")
            .get_to(c.replacements_after_undesirable_node_extractions);
        j.at("force_east_const_").get_to(c.force_east_const_);
        j.at("indent_depth_limit").get_to(c.indent_depth_limit);
    }

    namespace internal
    {
        static std::string CleanConfigurationExample = R"(
{
    "force_east_const_": true,
    "indent_depth_limit": 3,
    "replacements_after_undesirable_node_extractions": {
        "basic_string<_CharT, _Traits, _Allocator>": "std::string",
        "std::basic_string<char>": "std::string"
    },
    "suppress_custom_": [
        " __ptr64"
    ],
    "suppress_extra_namespaces_": [
        "::__1",
        "::__cxx11"
    ],
    "suppress_extract_struct_class_": [
        "class ",
        "struct "
    ],
    "undesirable_type_nodes_": [
        "std::char_traits",
        "std::allocator",
        "std::less"
    ]
}
    )";


        inline CleanConfiguration ImplGlobalConfig()
        {
            std::vector<std::string> parent_dirs = cleantype::filesystem::parent_directories();
            for (const std::string & dir : parent_dirs)
            {
                std::string pref_file = dir + "/.cleantype.json";
                if (cleantype::filesystem::file_exists(pref_file))
                {
                    std::ifstream is(pref_file);
                    nlohmann::json json_data;
                    is >> json_data;
                    cleantype::CleanConfiguration config =
                        json_data.get<cleantype::CleanConfiguration>();
                    return config;
                }
            }
            return cleantype::CleanConfiguration();
        }
    }  // namespace internal

}  // namespace cleantype

#endif  // #ifndef CLEANTYPE_USE_NLOHMANN_JSON



//////////////////////////////////////////
////   Header: details/cleantype_clean.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

//////////////////////////////////////////
////   Header: details/cleantype_clean_impl.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

//////////////////////////////////////////
////   Header: details/cleantype_fp/fp_tree.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

// These functions parse and manipulate trees in a functional way (this is the basis
// of the type parsing in this project)

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


//////////////////////////////////////////
////   Header: details/cleantype_fp/fp_show.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

// These functions are inspired by
// (FunctionalPlus)[https://github.com/Dobiasd/FunctionalPlus], a Functional
// Programming Library for C++, which I highly recommend.
//
// See its api search site (a la Haskell):
// http://www.editgym.com/fplus-api-search/ and see demo at
// https://code-ballads.net/generated-notebooks/cpp/repl_cling/markdown/#A-REPL-session-of-C++-functional-programming,-using-fplus


#include <algorithm>
#include <deque>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace cleantype_fp
{
    template <typename T>
    std::string show(const T & v)
    {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }
    inline std::string show(const bool & v) { return v ? "true" : "false"; }
    inline std::string show(const char * xs) { return std::string(xs); }

    template <typename T>
    std::string show_cont(T & xs);

}  // namespace cleantype_fp

// Below : overload for common containers
// (tuple is not yet included)

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cleantype_fp
{
    template <typename T, std::size_t N>
    std::string show(const std::array<T, N> & v)
    {
        return show_cont(v);
    }
    template <typename T>
    std::string show(const std::vector<T> & v)
    {
        return show_cont(v);
    }
    template <typename T>
    std::string show(const std::deque<T> & v)
    {
        return show_cont(v);
    }
    template <typename T, typename U>
    std::string show(const std::map<T, U> & v)
    {
        return show_cont(v);
    }

    template <typename T>
    std::string show(const std::forward_list<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::list<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::stack<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::queue<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::priority_queue<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }

    template <typename T>
    std::string show(const std::set<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::multiset<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }

    template <typename T, typename U>
    std::string show(const std::multimap<T, U> & v)
    {
        return cleantype_fp::show_cont(v);
    }

    template <typename T>
    std::string show(const std::unordered_set<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T>
    std::string show(const std::unordered_multiset<T> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T, typename U>
    std::string show(const std::unordered_map<T, U> & v)
    {
        return cleantype_fp::show_cont(v);
    }
    template <typename T, typename U>
    std::string show(const std::unordered_multimap<T, U> & v)
    {
        return cleantype_fp::show_cont(v);
    }

    template <typename T, typename U>
    std::string show(const std::pair<T, U> & xs)
    {
        return std::string("(") + show(xs.first) + ", " + show(xs.second) + ")";
    }

    template <typename T>
    std::string show_cont(T & xs)
    {
        std::vector<std::string> strs;
        for (const auto & x : xs)
            strs.push_back(cleantype_fp::show(x));

        return std::string("[") + cleantype_fp::join(", ", strs) + "]";
    }

}  // namespace cleantype_fp


namespace cleantype_fp_tree
{
    template <typename T>
    struct tree
    {
        tree(const T & value, const std::vector<tree<T>> & children)
            : value_(value), children_(children)
        {
        }
        T value_;
        std::vector<tree<T>> children_;
    };

    struct lhs_rhs
    {
        std::string lhs;
        std::string rhs;
    };

    using lhs_rhs_tree = tree<lhs_rhs>;

    struct tree_separators
    {
        char open_child = '<';
        char close_child = '>';
        char siblings_separator = ',';
    };

    enum class siblings_spacing_t
    {
        no_spacing,
        space,
        new_line
    };

    enum class children_indent_t
    {
        no_indent,
        newline_before_open_child,
        newline_before_child
    };

    struct show_tree_lhs_rhs_options
    {
        children_indent_t children_indent;
        bool add_space_after_lhs = false;
        bool add_space_before_rhs = false;
        siblings_spacing_t siblings_spacing = siblings_spacing_t::no_spacing;
        std::string indent = "  ";
    };

    inline lhs_rhs_tree parse_lhs_rhs_tree(std::string const & s,
                                           const tree_separators & separators,
                                           bool remove_space_after_separator)
    {
        bool is_filling_rhs = false;

        std::deque<char> fifo_letters;
        {
            for (const auto & letter : s)
                fifo_letters.push_back(letter);
        }

        lhs_rhs_tree result{{}, {}};
        lhs_rhs_tree * current = &result;
        std::vector<lhs_rhs_tree *> parents;

        bool is_first_after_separator = false;
        while (!fifo_letters.empty())
        {
            char c = fifo_letters.front();

            fifo_letters.pop_front();
            if (isspace(c))
            {
                if (remove_space_after_separator && is_first_after_separator)
                    continue;
            }
            if (c == separators.open_child)
            {
                lhs_rhs_tree child{{}, {}};
                current->children_.push_back(child);
                parents.push_back(current);
                current = &(parents.back()->children_.back());
                is_first_after_separator = true;
                is_filling_rhs = false;
            }
            else if (c == separators.siblings_separator)
            {
                lhs_rhs_tree brother{{}, {}};
                parents.back()->children_.push_back(brother);
                current = &(parents.back()->children_.back());
                is_first_after_separator = true;
                is_filling_rhs = false;
            }
            else if (c == separators.close_child)
            {
                current = parents.back();
                parents.pop_back();
                is_first_after_separator = true;
                is_filling_rhs = true;
            }
            else
            {
                if (is_filling_rhs)
                    current->value_.rhs += c;
                else
                    current->value_.lhs += c;
                is_first_after_separator = false;
            }
        }
        return result;
    }

    template <typename T>
    tree<T> tree_keep_if(std::function<bool(const T &)> f, const tree<T> & xs)
    {
        tree<T> result = xs;

        result.children_ = cleantype_fp::keep_if([f](const tree<T> & xxs) { return f(xxs.value_); },
                                                 result.children_);

        std::vector<tree<T>> children_filtered_grandchhildren;
        for (auto & child : result.children_)
        {
            auto child_filtered = tree_keep_if(f, child);
            children_filtered_grandchhildren.push_back(child_filtered);
        }
        result.children_ = children_filtered_grandchhildren;
        return result;
    }

    template <typename T, typename Transformer_T>
    void tree_transform_leafs_depth_first_inplace(Transformer_T transformer, tree<T> & xs_io)
    {
        for (auto & child : xs_io.children_)
            tree_transform_leafs_depth_first_inplace(transformer, child);
        transformer(xs_io.value_);
    }

    template <typename T, typename Transformer_T>
    void tree_transform_leafs_breadth_first_inplace(Transformer_T transformer, tree<T> & xs_io)
    {
        transformer(xs_io.value_);
        for (auto & child : xs_io.children_)
            tree_transform_leafs_breadth_first_inplace(transformer, child);
    }

    namespace detail
    {
        template <typename T>
        std::size_t tree_depth_impl(const tree<T> & xs, int current_depth)
        {
            std::vector<std::size_t> sizes;
            sizes.push_back(current_depth);
            for (auto & child : xs.children_)
                sizes.push_back(tree_depth_impl(child, current_depth + 1));
            return cleantype_fp::maximum(sizes);
        }
    }  // namespace detail

    template <typename T>
    std::size_t tree_depth(const tree<T> & xs)
    {
        return detail::tree_depth_impl(xs, 0);
    }

    template <typename OutputType, typename InputType, typename F>
    std::vector<OutputType> transform_vector(F f, const std::vector<InputType> & xs)
    {
        std::vector<OutputType> out;
        for (const auto & v : xs)
        {
            out.push_back(f(v));
        }
        return out;
    }

    template <typename T>
    std::string show_tree_children(const std::vector<tree<T>> & children,
                                   const tree_separators & separators,
                                   const show_tree_lhs_rhs_options & options,
                                   int level = 0)
    {
        std::vector<std::string> children_strs = transform_vector<std::string>(
            [=](const tree<T> & vv) -> std::string {
                return show_tree_lhs_rhs(vv, separators, options, level + 1);
            },
            children);

        const std::string siblings_separator = [&] {
            std::string separator_string(1, separators.siblings_separator);
            if (options.siblings_spacing == siblings_spacing_t::no_spacing)
                return separator_string;
            else if (options.siblings_spacing == siblings_spacing_t::space)
                return separator_string + " ";
            else
            {  // if (options.siblings_spacing == siblings_spacing_t::new_line)
                return separator_string + std::string("\n");
            }
        }();

        std::string children_str = cleantype_fp::join(siblings_separator, children_strs);
        return children_str;
    }

    template <typename T>
    std::string show_tree_lhs_rhs(const tree<T> & v,
                                  const tree_separators & separators,
                                  const show_tree_lhs_rhs_options & options,
                                  int level = 0)
    {
        auto line_start = cleantype_fp::repeat(level, options.indent);

        std::string result;
        if (options.children_indent != cleantype_fp_tree::children_indent_t::no_indent)
            result = line_start;

        result += cleantype_fp::show(v.value_.lhs);
        if (!v.value_.lhs.empty() && options.add_space_after_lhs)
            result += " ";

        if (!v.children_.empty())
        {
            if (options.children_indent == children_indent_t::newline_before_open_child)
                result += "\n" + line_start;
            result += separators.open_child;
            if (options.children_indent != children_indent_t::no_indent)
                result += "\n";

            std::string children_str = show_tree_children(v.children_, separators, options, level);

            result += children_str;

            if (options.children_indent != children_indent_t::no_indent)
                result += "\n" + line_start;
            result += separators.close_child;
        }

        if (!v.value_.rhs.empty() && options.add_space_before_rhs)
            result += " ";
        result += cleantype_fp::show(v.value_.rhs);

        return result;
    }

}  // namespace cleantype_fp_tree


//////////////////////////////////////////
////   Header: details/cleantype_full.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

//////////////////////////////////////////
////   Header: details/cleantype_eastconst.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)
#include <deque>
#include <string>

namespace cleantype
{
    namespace internal
    {
        std::vector<std::string> split_types(std::string const & type_names);

        inline std::string apply_east_const_one_type(std::string const & type_name)
        {
            // Note : this implementation is by no means neither complete nor foolproof
            // It expects types that were preprocessed as inputs (spaces before * and &, etc.)
            // For a more complete implementation, maybe a BNF grammar parsing would be required
            //
            // By default, this transformation is not applied : it is only for the unit tests
            // consistency accross platforms. I am not an east-const zealot.
            namespace stringutils = cleantype::stringutils;

            if (type_name.empty())
                return "";
            if (stringutils::ends_with(type_name, "const") &&
                (!stringutils::starts_with(type_name, "const ")))
                return type_name;

            // const T * const => T const * const
            if (stringutils::starts_ends_with(type_name, "const ", " * const"))
            {
                auto r = stringutils::remove_start_end(type_name, "const ", " * const");
                r = r + " const * const";
                return r;
            }

            // const T * const & => T const * const &
            if (stringutils::starts_ends_with(type_name, "const ", " * const &"))
            {
                auto r = stringutils::remove_start_end(type_name, "const ", " * const &");
                r = r + " const * const &";
                return r;
            }

            // const T * & => T const * &
            if (stringutils::starts_ends_with(type_name, "const ", " * &"))
            {
                auto r = stringutils::remove_start_end(type_name, "const ", " * &");
                r = r + " const * &";
                return r;
            }

            // const T & => T const &
            if (stringutils::starts_ends_with(type_name, "const ", " &"))
            {
                auto r = stringutils::remove_start_end(type_name, "const ", " &");
                r = r + " const &";
                return r;
            }

            // const T * => T const *
            if (stringutils::starts_ends_with(type_name, "const ", " *"))
            {
                auto r = stringutils::remove_start_end(type_name, "const ", " *");
                r = r + " const *";
                return r;
            }

            // const * T => T const *
            if (stringutils::starts_with(type_name, "const * "))
            {
                auto r = stringutils::remove_start(type_name, "const * ");
                r = r + " const *";
                return r;
            }

            // const T => T const
            if (stringutils::starts_with(type_name, "const "))
            {
                auto r = stringutils::remove_start(type_name, "const ");
                r = r + " const";
                return r;
            }

            return type_name;
        }

    }  // namespace internal

    inline std::string apply_east_const_typelist(std::string const & type_names)
    {
        std::vector<std::string> types = internal::split_types(type_names);
        types = cleantype_fp::transform(internal::apply_east_const_one_type, types);
        std::string r = cleantype_fp::join(std::string(", "), types);
        return r;
    }

}  // namespace cleantype


//////////////////////////////////////////
////   Header: details/cleantype_format.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)
#include <string>

namespace cleantype
{
    namespace internal
    {
        // r = stringutils::replace_tokens("__cdecl ", "", r);
        // r = stringutils::replace_tokens("struct ", "", r);
        // r = stringutils::replace_tokens("class ", "", r);

        inline std::string format_whitespace(const std::string & str_type)
        {
            std::string r = str_type;
            r = stringutils::replace_tokens("\t", " ", r);
            r = stringutils::replace_tokens("\r\n", " ", r);
            r = stringutils::replace_tokens("\n", " ", r);
            r = stringutils::replace_tokens("  ", " ", r);
            r = stringutils::replace_tokens("  ", " ", r);
            r = stringutils::replace_tokens("  ", " ", r);
            r = stringutils::insert_spaces_after(',', r);
            r = stringutils::insert_spaces_before_after('&', r);
            r = stringutils::insert_spaces_before_after('*', r);
            r = stringutils::remove_spaces_before_after(')', r);
            r = stringutils::remove_spaces_before_after('(', r);
            r = stringutils::remove_spaces_before('>', r);
            r = stringutils::remove_spaces_after('<', r);
            r = stringutils::remove_spaces_after('[', r);
            r = stringutils::remove_spaces_before(']', r);
            r = stringutils::insert_spaces_before('[', r);
            r = stringutils::insert_spaces_after(']', r);
            r = stringutils::replace_tokens("*&", "* &", r);
            r = stringutils::replace_tokens("&*", "& *", r);
            r = stringutils::replace_tokens("& &", "&&", r);
            r = stringutils::replace_tokens("[ ]", "[]", r);
            r = stringutils::replace_tokens(" ,", ",", r);
            r = stringutils::replace_tokens("* *", "**", r);
            r = cleantype_fp::trim(' ', r);
            return r;
        }

    }  // namespace internal

}  // namespace cleantype


//////////////////////////////////////////
////   Header: details/cleantype_holder.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)
#include <string>

namespace cleantype
{
    namespace internal
    {
        // Trick in order to avoid having to deal the tedious syntax of parameter packs
        template <typename... T>
        struct TupleTypeHolder
        {
        };

        inline std::string add_type_holder_str(const std::string & type_names)
        {
#ifdef _MSC_VER
            const std::string start = "struct cleantype::internal::TupleTypeHolder<";
#else
            const std::string start = "cleantype::internal::TupleTypeHolder<";
#endif
            const std::string end = ">";
            return start + type_names + end;
        }

        inline std::string remove_type_holder_str(const std::string & types_inside_holder)
        {
            std::string r = types_inside_holder;
            if (stringutils::starts_with(r, "struct cleantype::internal::TupleTypeHolder<"))
                r = stringutils::remove_start(r, "struct cleantype::internal::TupleTypeHolder<");
            if (stringutils::starts_with(r, "cleantype::internal::TupleTypeHolder<"))
                r = stringutils::remove_start(r, "cleantype::internal::TupleTypeHolder<");
            if (stringutils::ends_with(r, ">"))
                r = stringutils::remove_end(r, ">");
            return r;
        }

        // splits a string containings a list of comma separated types into
        // a vector of types
        // example :
        // split_types("int, map<char, int>") => ["int", "map<char, int>"]
        inline std::vector<std::string> split_types(std::string const & type_names)
        {
            std::vector<std::string> result;
            // counts < and > occurrences
            int count = 0;
            std::string current;
            for (const auto c : type_names)
            {
                if (c == '<')
                    ++count;
                if (c == '>')
                    --count;
                if ((c == ',') && (count == 0))
                {
                    result.push_back(cleantype_fp::trim(' ', current));
                    current = "";
                }
                else
                {
                    current += c;
                }
            }
            result.push_back(cleantype_fp::trim(' ', current));
            return result;
        }

    }  // namespace internal
}  // namespace cleantype

//////////////////////////////////////////
////   Header: details/hana_type_name/type_name_pretty_function.hpp
//////////////////////////////////////////
/*
@file
Defines `boost::hana::experimental::type_name`.
@copyright Louis Dionne 2013-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

/*
This folder contains is a proposed extension
to [Boost.Hana](http://boostorg.github.io/hana/)'s[experimental type_name]

The original version was experimental, and limited to Clang.
This version:
* adds complete support for MSVC
* adds partial support for GCC (runtime only)
* offers compiler capabilities detection
* runs out of the box with or without boost

This extension is being studied :
see PR  https://github.com/boostorg/hana/pull/432
which originates from the author of this project (Pascal Thomet).
*/

#ifndef BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_PRERRY_FUNCTION_HPP
#define BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_PRERRY_FUNCTION_HPP


//////////////////////////////////////////
////   Header: details/hana_type_name/type_name_compiler_capabilities.hpp
//////////////////////////////////////////
/*
@file
Defines `boost::hana::experimental::type_name`.
@copyright Louis Dionne 2013-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

/*
This folder contains is a proposed extension
to [Boost.Hana](http://boostorg.github.io/hana/)'s[experimental type_name]

The original version was experimental, and limited to Clang.
This version:
* adds complete support for MSVC
* adds partial support for GCC (runtime only)
* offers compiler capabilities detection
* runs out of the box with or without boost

This extension is being studied :
see PR  https://github.com/boostorg/hana/pull/432
which originates from the author of this project (Pascal Thomet).
*/

#ifndef BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_COMPILER_CAPABILITIES_HPP
#define BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_COMPILER_CAPABILITIES_HPP

// only clang and MSVC support constexpr __PRETTY_FUNCTION__, gcc does not
#if defined(__clang__) || defined(_MSC_VER)
#define _HANA_TN_CAN_CONSTEXPR
#endif

// in constexpr mode, strlen is equivalent to sizeof() - 1
#ifdef _HANA_TN_CAN_CONSTEXPR
#define _HANA_TN_CONSTEXPR_IF_POSSIBLE constexpr
#define _HANA_SIZEOF_OR_STRLEN(var) sizeof(var) - 1
#else
#include <cstring>  // this include is not needed in constexpr mode, save compilation time
#define _HANA_TN_CONSTEXPR_IF_POSSIBLE
#define _HANA_SIZEOF_OR_STRLEN(var) strlen(var)
#endif

#endif  // !BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_COMPILER_CAPABILITIES_HPP


//////////////////////////////////////////
////   Header: details/hana_type_name/type_name_stringliteral.hpp
//////////////////////////////////////////
/*
@file
Defines `boost::hana::experimental::type_name`.
@copyright Louis Dionne 2013-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

/*
This folder contains is a proposed extension
to [Boost.Hana](http://boostorg.github.io/hana/)'s[experimental type_name]

The original version was experimental, and limited to Clang.
This version:
* adds complete support for MSVC
* adds partial support for GCC (runtime only)
* offers compiler capabilities detection
* runs out of the box with or without boost

This extension is being studied :
see PR  https://github.com/boostorg/hana/pull/432
which originates from the author of this project (Pascal Thomet).
*/

#ifndef BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_CSTRING_HPP
#define BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_CSTRING_HPP

#include <cstddef>
#include <cstring>
#include <string>

namespace boost
{
    namespace hana
    {
        namespace experimental
        {
            namespace type_name_details
            {
                struct stringliteral
                {
                    char const * ptr;
                    std::size_t length;
                };

#define _HANA_TN_MAKE_STRINGLITERAL(str_literal) \
    stringliteral                                \
    {                                            \
        str_literal, _HANA_SIZEOF_OR_STRLEN(str_literal);

                inline constexpr std::size_t constexpr_strlen(char const * s)
                {
                    std::size_t r = 0;
                    while (*s++ != '\0')
                        r++;
                    return r;
                }

                inline constexpr bool stringliteral_equal(stringliteral const & cs1,
                                                          stringliteral const & cs2)
                {
                    if (cs1.length != cs2.length)
                        return false;

                    std::size_t idx = 0;
                    do
                    {
                        char c1 = cs1.ptr[idx];
                        char c2 = cs2.ptr[idx];
                        if (c1 != c2)
                            return false;
                        idx++;
                    } while (idx < cs1.length);
                    return true;
                }

                inline constexpr bool stringliteral_equal_sz(stringliteral const & cs1,
                                                             char const * literal)
                {
                    stringliteral cs2{literal, constexpr_strlen(literal)};
                    return stringliteral_equal(cs1, cs2);
                }

                inline std::string stringliteral_to_string(stringliteral const & cs)
                {
                    return std::string(cs.ptr, cs.length);
                }

            }  // namespace type_name_details
        }      // namespace experimental
    }          // namespace hana
}  // namespace boost

#endif  // !BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_CSTRING_HPP

#include <cstddef>

#ifdef _MSC_VER
#define _HANA_TN__PRETTY_FUNCTION__ __FUNCSIG__
#else
#define _HANA_TN__PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif

#if defined(__clang__)
#define _HANA_TN_PRETTY_FUNCTION_TYPE_PREFIX                       \
    "boost::hana::experimental::type_name_details::stringliteral " \
    "boost::hana::experimental::type_name_details::type_name_impl_stringliteral() [T = "
#define _HANA_TN_PRETTY_FUNCTION_TYPE_SUFFIX "]"
#elif defined(_MSC_VER)
#define _HANA_TN_PRETTY_FUNCTION_TYPE_PREFIX                                      \
    "struct boost::hana::experimental::type_name_details::stringliteral __cdecl " \
    "boost::hana::experimental::type_name_details::type_name_impl_stringliteral<"
#define _HANA_TN_PRETTY_FUNCTION_TYPE_SUFFIX ">(void)"
#elif defined(__GNUC__) || defined(__GNUG__)
#define _HANA_TN_PRETTY_FUNCTION_TYPE_PREFIX                                 \
    "constexpr boost::hana::experimental::type_name_details::stringliteral " \
    "boost::hana::experimental::type_name_details::type_name_impl_stringliteral() [with T = "
#define _HANA_TN_PRETTY_FUNCTION_TYPE_SUFFIX "]"
#else
#error "No support for this compiler."
#endif

namespace boost
{
    namespace hana
    {
        namespace experimental
        {
            namespace type_name_details
            {
                template <typename T>
                constexpr stringliteral type_name_impl_stringliteral()
                {
                    _HANA_TN_CONSTEXPR_IF_POSSIBLE char const * pretty_function =
                        _HANA_TN__PRETTY_FUNCTION__;
                    _HANA_TN_CONSTEXPR_IF_POSSIBLE std::size_t total_size =
                        _HANA_SIZEOF_OR_STRLEN(_HANA_TN__PRETTY_FUNCTION__);
                    _HANA_TN_CONSTEXPR_IF_POSSIBLE std::size_t prefix_size =
                        _HANA_SIZEOF_OR_STRLEN(_HANA_TN_PRETTY_FUNCTION_TYPE_PREFIX);
                    _HANA_TN_CONSTEXPR_IF_POSSIBLE std::size_t suffix_size =
                        _HANA_SIZEOF_OR_STRLEN(_HANA_TN_PRETTY_FUNCTION_TYPE_SUFFIX);
                    return {pretty_function + prefix_size, total_size - prefix_size - suffix_size};
                    // return {pretty_function, total_size};
                }
            }  // end namespace type_name_details

        }  // namespace experimental
    }      // namespace hana
}  // namespace boost

#endif  // !BOOST_HANA_EXPERIMENTAL_DETAIL_TYPE_NAME_PRERRY_FUNCTION_HPP

#include <cstdlib>
#include <memory>
#include <string>

namespace cleantype
{
    using stringliteral = boost::hana::experimental::type_name_details::stringliteral;
    namespace internal
    {
        template <typename T>
        constexpr stringliteral _impl_typeid_hana_literal()
        {
            return boost::hana::experimental::type_name_details::type_name_impl_stringliteral<T>();
        }

        template <typename T>
        std::string _impl_typeid_hana()
        {
            return boost::hana::experimental::type_name_details::stringliteral_to_string(
                _impl_typeid_hana_literal<T>());
        }

        template <typename... T>
        constexpr stringliteral _impl_typeid_hana_inside_holder()
        {
            return boost::hana::experimental::type_name_details::type_name_impl_stringliteral<
                TupleTypeHolder<T...>>();
        }

        template <typename... T>
        stringliteral impl_typeid()
        {
            return _impl_typeid_hana_inside_holder<T...>();
        }

        template <typename... T>
        std::string impl_full()
        {
            stringliteral type_sl_in_tupletypeholder = impl_typeid<T...>();
            std::string type_in_tupletypeholder =
                boost::hana::experimental::type_name_details::stringliteral_to_string(
                    type_sl_in_tupletypeholder);

            std::string type_definition = remove_type_holder_str(type_in_tupletypeholder);

            std::string formatted = cleantype::internal::format_whitespace(type_definition);

            if (CleanConfiguration::GlobalConfig().force_east_const_)
                formatted = cleantype::apply_east_const_typelist(formatted);

            return formatted;
        }
    }  // namespace internal

    template <typename... T>
    std::string full()
    {
        return internal::impl_full<T...>();
    }

    template <typename... T>
    std::string full(T &&... v)
    {
        return full<T...>();
    }

    template <typename T>
    std::string show_details_full(T && v)
    {
        return std::string("[") + full<T>() + "]" + " = " + cleantype_fp::show(v);
    }

}  // namespace cleantype

#define CT_cleantype_full(var) cleantype::full<decltype(var)>()

#define CT_show_details_full(var) \
    std::string("[") + CT_cleantype_full(var) + "] " + #var + " = " + cleantype_fp::show(var)

#define CT_show_details_full_cont(var) \
    std::string("[") + CT_cleantype_full(var) + "] " + #var + " = " + cleantype_fp::show_cont(var)

#define CT_compiler_log_var_type(var) \
    {                                 \
        var.IntentionalError = 42;    \
    }
#define CT_compiler_log_type(T)   \
    {                             \
        T::IntentionalError = 42; \
    }


//////////////////////////////////////////
////   Header: details/cleantype_parse.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)
#include <string>

namespace cleantype
{
    namespace internal
    {
        // lhs / rhs explanation :
        // for example in "std::vector<int> const &",
        // - lhs = "std::vector"
        // - rhs = "const &"
        // - int is the lhs of a child node
        using code_pair = cleantype_fp_tree::lhs_rhs;
        using code_pair_tree = cleantype_fp_tree::tree<code_pair>;

        inline cleantype_fp_tree::tree_separators make_template_tree_separators()
        {
            cleantype_fp_tree::tree_separators sep;
            sep.open_child = '<';
            sep.close_child = '>';
            sep.siblings_separator = ',';
            return sep;
        }

        inline cleantype_fp_tree::show_tree_lhs_rhs_options make_template_show_tree_options_impl(
            bool indent)
        {
            cleantype_fp_tree::show_tree_lhs_rhs_options result;
            if (indent)
            {
                result.children_indent = cleantype_fp_tree::children_indent_t::newline_before_child;
                result.siblings_spacing = cleantype_fp_tree::siblings_spacing_t::new_line;
                result.indent = "    ";
            }
            else
            {
                result.children_indent = cleantype_fp_tree::children_indent_t::no_indent;
                result.siblings_spacing = cleantype_fp_tree::siblings_spacing_t::space;
                result.indent = "";
            }
            result.add_space_after_lhs = false;
            result.add_space_before_rhs = true;
            return result;
        }

        inline cleantype_fp_tree::show_tree_lhs_rhs_options
        make_template_show_tree_options_no_indent()
        {
            return make_template_show_tree_options_impl(false);
        }

        inline cleantype_fp_tree::show_tree_lhs_rhs_options
        make_template_show_tree_options_with_indent()
        {
            return make_template_show_tree_options_impl(true);
        }

        inline code_pair_tree parse_template_tree(std::string const & s)
        {
            return parse_lhs_rhs_tree(s, make_template_tree_separators(), true);
        }

        inline std::string code_pair_tree_to_string(code_pair_tree const & xs)
        {
            return cleantype_fp_tree::show_tree_lhs_rhs(
                xs, make_template_tree_separators(), make_template_show_tree_options_no_indent());
        }

    }  // namespace internal

}  // namespace cleantype

#include <deque>
#include <string>

namespace cleantype
{
    namespace internal
    {
        inline code_pair_tree filter_undesirable_type_nodes(code_pair_tree const & xs)
        {
            std::function<bool(const code_pair &)> is_node_desirable =
                [](const code_pair & code_pair) {
                    const std::vector<std::string> undesirable_nodes =
                        cleantype::CleanConfiguration::GlobalConfig().undesirable_type_nodes_;
                    bool found = std::find(undesirable_nodes.begin(),
                                           undesirable_nodes.end(),
                                           cleantype_fp::trim(' ', code_pair.lhs)) !=
                                 undesirable_nodes.end();
                    return !found;
                };

            auto xss = cleantype_fp_tree::tree_keep_if(is_node_desirable, xs);
            return xss;
        }

        inline std::string perform_suppressions(std::string const & typ_name,
                                                std::vector<std::string> const & suppressions)
        {
            std::string result = typ_name;
            for (const auto & v : suppressions)
                result = cleantype_fp::replace_tokens(v, std::string(""), result);
            return result;
        }

        inline std::string perform_replacements(
            std::string const & typ_name, std::map<std::string, std::string> const & replacements)
        {
            std::string result = typ_name;
            for (const auto & kv : replacements)
                result = cleantype_fp::replace_tokens(kv.first, kv.second, result);
            return result;
        }

        inline std::string remove_extra_namespaces(std::string const & typ_name)
        {
            return perform_suppressions(
                typ_name, cleantype::CleanConfiguration::GlobalConfig().suppress_extra_namespaces_);
        }

        inline std::string remove_struct_class(std::string const & typ_name)
        {
            return perform_suppressions(
                typ_name,
                cleantype::CleanConfiguration::GlobalConfig().suppress_extract_struct_class_);
        }

        inline std::string remove_custom(std::string const & typ_name)
        {
            return perform_suppressions(
                typ_name, cleantype::CleanConfiguration::GlobalConfig().suppress_custom_);
        }

        inline std::string perform_std_replacements(std::string const & typ_name)
        {
            return perform_replacements(typ_name,
                                        cleantype::CleanConfiguration::GlobalConfig()
                                            .replacements_after_undesirable_node_extractions);
        }

        inline void trim_spaces_inplace(code_pair & xs_io)
        {
            xs_io.lhs = cleantype_fp::trim(' ', xs_io.lhs);
            xs_io.rhs = cleantype_fp::trim(' ', xs_io.rhs);
        }

        inline std::string impl_clean_one_type(std::string const & typ_name)
        {
            std::string typ_namecleaned = remove_struct_class(remove_extra_namespaces(typ_name));
            typ_namecleaned = remove_custom(typ_namecleaned);

            code_pair_tree template_tree = parse_template_tree(typ_namecleaned);
            cleantype_fp_tree::tree_transform_leafs_depth_first_inplace(trim_spaces_inplace,
                                                                        template_tree);
            code_pair_tree template_tree_filtered = filter_undesirable_type_nodes(template_tree);
            std::string template_tree_filtered_str =
                code_pair_tree_to_string(template_tree_filtered);

            std::string final_type = perform_std_replacements(template_tree_filtered_str);
            final_type = format_whitespace(final_type);
            return final_type;
        }

        inline std::string impl_indent_type_tree(const std::string & type_names)
        {
            std::string types_with_holder = add_type_holder_str(type_names);
            code_pair_tree template_tree = parse_template_tree(types_with_holder);
            std::string types_with_holder_indented =
                cleantype_fp_tree::show_tree_lhs_rhs(template_tree,
                                                     make_template_tree_separators(),
                                                     make_template_show_tree_options_with_indent());

            // now, types_with_holder_indented is like this:
            //
            // cleantype::internal::TupleTypeHolder<
            //     Foo<
            //         int,
            //         char
            //     >
            // >
            //
            // --> we remove the lines [0, last]
            //     then we remove the first indentation level
            auto remove_indented_tuple_holder = [](const std::string & type_str) {
                std::vector<std::string> lines = stringutils::split_string(type_str, '\n');
                assert(lines.size() >= 3);
                std::vector<std::string> filtered_lines(lines.begin() + 1, lines.end() - 1);
                std::vector<std::string> unindented_lines = cleantype_fp::transform(
                    [](const std::string & s) { return stringutils::remove_start(s, "    "); },
                    filtered_lines);
                std::string joined_lines = cleantype_fp::join(std::string("\n"), unindented_lines);
                return joined_lines;
            };
            return remove_indented_tuple_holder(types_with_holder_indented);
        }

        inline std::string impl_clean_several_types(std::string const & type_names)
        {
            std::string types_with_holder = add_type_holder_str(type_names);
            std::string types_clean_with_holder = impl_clean_one_type(types_with_holder);
            return remove_type_holder_str(types_clean_with_holder);
        }

        inline std::string impl_indent_if_neeeded(std::string const & type_names)
        {
            std::size_t indent_depth_limit =
                cleantype::CleanConfiguration::GlobalConfig().indent_depth_limit;
            if (indent_depth_limit == 0)
                return type_names;
            std::string types_with_holder = add_type_holder_str(type_names);
            code_pair_tree template_tree = parse_template_tree(types_with_holder);
            std::size_t depth = cleantype_fp_tree::tree_depth(template_tree);
            if (depth > indent_depth_limit + 1)
                return impl_indent_type_tree(type_names);
            else
                return type_names;
        }

        inline std::string impl_clean(std::string const & type_names)
        {
            std::string cleaned = impl_clean_several_types(type_names);
            std::string indented = impl_indent_if_neeeded(cleaned);
            return indented;
        }

    }  // namespace internal

}  // namespace cleantype

#include <deque>
#include <string>

namespace cleantype
{
    inline std::string clean_typestring(const std::string & type_names)
    {
        return internal::impl_clean(type_names);
    }

    inline std::string indent_type_tree(const std::string & type_names)
    {
        return internal::impl_indent_type_tree(type_names);
    }

    template <typename... T>
    std::string clean()
    {
        return clean_typestring(internal::impl_full<T...>());
    }

    template <typename... T>
    std::string clean(T &&... v)
    {
        return clean<T...>();
    }

    template <class T>
    std::string show_details(T && v)
    {
        return clean<T>() + " = " + cleantype_fp::show(v);
    }

}  // namespace cleantype

#define CT_cleantype_clean(var) cleantype::clean<decltype(var)>()

#define CT_show_details(var) \
    std::string("[") + CT_cleantype_clean(var) + "] " + #var + " = " + cleantype_fp::show(var)

#define CT_show_details_cont(var) \
    std::string("[") + CT_cleantype_clean(var) + "] " + #var + " = " + cleantype_fp::show_cont(var)


//////////////////////////////////////////
////   Header: details/cleantype_invoke_result.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)


//////////////////////////////////////////
////   Header: details/invoke_result_polyfill.hpp
//////////////////////////////////////////
#include <functional>
// From https://en.cppreference.com/w/cpp/types/result_of

namespace cleantype
{
#if __cplusplus < 201703L
    namespace detail
    {
        template <class T>
        struct is_reference_wrapper : std::false_type
        {
        };
        template <class U>
        struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
        {
        };

        template <class T>
        struct invoke_impl
        {
            template <class F, class... Args>
            static auto call(F && f, Args &&... args)
                -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        };

        template <class B, class MT>
        struct invoke_impl<MT B::*>
        {
            template <class T,
                      class Td = typename std::decay<T>::type,
                      class = typename std::enable_if<std::is_base_of<B, Td>::value>::type>
            static auto get(T && t) -> T &&;

            template <class T,
                      class Td = typename std::decay<T>::type,
                      class = typename std::enable_if<is_reference_wrapper<Td>::value>::type>
            static auto get(T && t) -> decltype(t.get());

            template <class T,
                      class Td = typename std::decay<T>::type,
                      class = typename std::enable_if<!std::is_base_of<B, Td>::value>::type,
                      class = typename std::enable_if<!is_reference_wrapper<Td>::value>::type>
            static auto get(T && t) -> decltype(*std::forward<T>(t));

            template <class T,
                      class... Args,
                      class MT1,
                      class = typename std::enable_if<std::is_function<MT1>::value>::type>
            static auto call(MT1 B::*pmf, T && t, Args &&... args)
                -> decltype((invoke_impl::get(std::forward<T>(t)).*
                             pmf)(std::forward<Args>(args)...));

            template <class T>
            static auto call(MT B::*pmd, T && t)
                -> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
        };

        template <class F, class... Args, class Fd = typename std::decay<F>::type>
        auto INVOKE(F && f, Args &&... args)
            -> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));

    }  // namespace detail

    // Conforming C++14 implementation (is also a valid C++11 implementation):
    namespace detail
    {
        template <typename AlwaysVoid, typename, typename...>
        struct invoke_result
        {
        };
        template <typename F, typename... Args>
        struct invoke_result<decltype(
                                 void(detail::INVOKE(std::declval<F>(), std::declval<Args>()...))),
                             F,
                             Args...>
        {
            using type = decltype(detail::INVOKE(std::declval<F>(), std::declval<Args>()...));
        };
    }  // namespace detail
#endif

}  // namespace cleantype

namespace cleantype
{
#if __cplusplus < 201703L
    template <class F, class... ArgTypes>
    struct invoke_result : detail::invoke_result<void, F, ArgTypes...>
    {
    };
#else
    template <class F, class... ArgTypes>
    struct invoke_result : std::invoke_result<F, ArgTypes...>
    {
    };
#endif

    template <class F, class... ArgTypes>
    using invoke_result_t = typename invoke_result<F, ArgTypes...>::type;
}  // namespace cleantype

#define CT_invoke_result_fn(f, ...) cleantype::invoke_result<decltype(&f), __VA_ARGS__>::type

#define CT_invoke_result_fn_template(f, ...) \
    cleantype::invoke_result<decltype(&f<__VA_ARGS__>), __VA_ARGS__>::type

#define CT_type_fn(f, ...) cleantype::clean<CT_invoke_result_fn(f, __VA_ARGS__)>()
#define CT_type_fn_full(f, ...) cleantype::full<CT_invoke_result_fn(f, __VA_ARGS__)>()

#define CT_type_fn_template(f, ...) cleantype::clean<CT_invoke_result_fn_template(f, __VA_ARGS__)>()
#define CT_type_fn_template_full(f, ...) \
    cleantype::full<CT_invoke_result_fn_template(f, __VA_ARGS__)>()

#ifdef _MSC_VER
// under MSVC 2017, std::invoke_result fails with template functions
// See
// https://stackoverflow.com/questions/54111146/invoke-result-for-template-function-with-auto-return-type-and-msvc-2017
// You need to call CT_invoke_result_fn_template_memoize before calling CT_invoke_result_fn_template
// in order to help the compiler
#define CT_invoke_result_fn_template_memoize(f, ...) \
    {                                                \
        auto _ = f<__VA_ARGS__>;                     \
        (void)_;                                     \
    }
#endif


//////////////////////////////////////////
////   Header: details/cleantype_lambda.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)

//////////////////////////////////////////
////   Header: details/cleantype_lambda_parse.hpp
//////////////////////////////////////////
// This file is part of cleantype: Clean Types for C++
// Copyright Pascal Thomet - 2018
// Distributed under the Boost Software License, Version 1.0. (see LICENSE.md)
#include <cassert>
#include <iostream>

namespace cleantype
{
    namespace internal
    {
        inline std::string remove_outer_parenthesis(std::string const & s)
        {
            assert(s.size() >= 2);
            assert(s.front() == '(');
            assert(s.back() == ')');
            std::string result(s.begin() + 1, s.end() - 1);
            return result;
        }

        struct extract_parenthesis_content_at_end_result
        {
            std::string remaining_at_start;
            std::string parenthesis_content;
            bool success;

            static extract_parenthesis_content_at_end_result error()
            {
                extract_parenthesis_content_at_end_result r;
                r.success = false;
                return r;
            }
        };

        // Example :
        //  "ABC(DEF)(GHI)KLM"
        // Returns { remaining_at_start = "ABC", parenthesis_content="GHI", success = true }
        inline extract_parenthesis_content_at_end_result extract_parenthesis_content_at_end(
            std::string const & str)
        {
            if (str.empty())
            {
                std::cerr << "extract_parenthesis_content_at_end_result : error (empty input)"
                          << std::endl;
                return extract_parenthesis_content_at_end_result::error();
            }

            extract_parenthesis_content_at_end_result result;

            std::string s = str;

            while (s.back() != ')')
            {
                s.pop_back();
                if (s.empty())
                {
                    std::cerr
                        << "extract_parenthesis_content_at_end_result: error (missing last ')' )"
                        << std::endl;
                    return extract_parenthesis_content_at_end_result::error();
                }
            }

            std::vector<char> content;
            int nb_parenthesis = 1;
            content.push_back(')');
            s.pop_back();
            while (nb_parenthesis > 0)
            {
                char c = s.back();
                content.push_back(c);
                if (c == ')')
                    nb_parenthesis++;
                if (c == '(')
                    nb_parenthesis--;
                s.pop_back();
                if (s.empty())
                {
                    std::cerr << "extract_parenthesis_content_at_end_result: error (non zero count "
                                 "of '()' )"
                              << std::endl;
                    return extract_parenthesis_content_at_end_result::error();
                }
            }
            std::reverse(content.begin(), content.end());
            for (auto c : content)
                result.parenthesis_content += c;

            result.parenthesis_content = remove_outer_parenthesis(result.parenthesis_content);
            result.remaining_at_start = s;
            result.success = true;
            return result;
        }

        inline std::string _remove_mem_fn_surround(std::string const & mem_fn_type)
        {
            std::string result = mem_fn_type;
            // Suppress mem_fn< at the start
            size_t idx1 = result.find('<');
            if (idx1 == std::string::npos)
                return "Error, can not find first '<' in mem_fn_type: " + mem_fn_type;
            result = result.substr(idx1 + 1);

            // Suppress all after the last ')'
            size_t idx2 = result.rfind(')');
            if (idx1 == std::string::npos)
                return "Error, can not find last '>' in mem_fn_type: " + mem_fn_type;
            result = result.substr(0, idx2 + 1);

            return result;
        }

        inline std::string _mem_fn_to_lambda_type(std::string const & mem_fn_type,
                                                  bool clean_params)
        {
            // Examples of possible inputs:

            // auto f = [&c](int a, int b) -> double { return a + b + c; };
            // MSVC : class std::_Mem_fn<double (__thiscall
            // <lambda_1d102738ade82cc35233c841173ca72c>::*)(int,int)const > clang:
            // std::__1::__mem_fn<double (cleantype::_DOCTEST_ANON_FUNC_2()::$_1::*)(int, int)
            // const> MSVC : double (__thiscall
            // <lambda_1d102738ade82cc35233c841173ca72c>::*)(int,int)const

            // auto f = [](int a, int b)  { return std::pair<int, int>(a, b); };
            // clang: std::__1::__mem_fn<std::__1::pair<int, int>
            // (cleantype::_DOCTEST_ANON_FUNC_2()::$_1::*)(int, int) const> clang:
            // std::__1::pair<int, int> (cleantype::_DOCTEST_ANON_FUNC_2()::$_1::*)(int, int) const

            const std::string lambda_full_type = _remove_mem_fn_surround(mem_fn_type);
            // std::cout << lambda_full_type << std::endl;
            std::string params_str, return_str_with_leading_garbage;
            {
                // lambda params are at the end between parenthesis
                auto params_r = extract_parenthesis_content_at_end(lambda_full_type);
                if (!params_r.success)
                    std::cerr << "_mem_fn_to_lambda_type : error parsing mem_fn_type --> "
                              << mem_fn_type << std::endl;
                params_str = params_r.parenthesis_content;
                return_str_with_leading_garbage = params_r.remaining_at_start;
            }

            // Separate params and clean them, then join them
            const std::string params_cleaned = [&]() {
                auto params_list = split_types(params_str);
                if (clean_params)
                    params_list = cleantype_fp::transform(impl_clean, params_list);
                std::string params_joined = cleantype_fp::join(std::string(", "), params_list);
                if (params_joined == "void")
                    params_joined = "";
                if (cleantype::CleanConfiguration::GlobalConfig().force_east_const_)
                    params_joined = cleantype::apply_east_const_typelist(params_joined);
                return params_joined;
            }();

            // garbage between the parentheses before (lambda anonymous name)
            std::string return_str;
            {
                auto garbage_r =
                    extract_parenthesis_content_at_end(return_str_with_leading_garbage);
                if (!garbage_r.success)
                    std::cerr << "_mem_fn_to_lambda_type : error parsing mem_fn_type --> "
                              << mem_fn_type << std::endl;
                return_str = garbage_r.remaining_at_start;
            }

            std::string return_type = clean_params ? impl_clean(return_str) : return_str;
            if (cleantype::CleanConfiguration::GlobalConfig().force_east_const_)
                return_type = cleantype::apply_east_const_typelist(return_type);

            // std::cout << "params= " << params << '\n';
            // std::cout << "return_type= " << return_type << '\n';
            return std::string("lambda: ") + "(" + params_cleaned + ")" + " -> " + return_type;
        }

    }  // namespace internal

}  // namespace cleantype


namespace cleantype
{
    namespace internal
    {
        template <typename First, typename... Rest, typename Lambda>
        std::string lambda_memfn_type(Lambda fn)
        {
#ifdef _MSC_VER
            auto as_ptr = &Lambda::operator()<First, Rest...>;
#else
            auto as_ptr = &Lambda::template operator()<First, Rest...>;
#endif
            auto as_mem_fn = std::mem_fn(as_ptr);
            std::string mem_fn_type = cleantype::internal::_impl_typeid_hana<decltype(as_mem_fn)>();

            return mem_fn_type;
        }

        template <typename Lambda>
        std::string lambda_memfn_type(Lambda fn)
        {
            auto as_ptr = &Lambda::operator();  // if you have an error here, your lambda is generic! Add template params for its input types!
            auto as_mem_fn = std::mem_fn(as_ptr);
            std::string mem_fn_type = cleantype::internal::_impl_typeid_hana<decltype(as_mem_fn)>();
            return mem_fn_type;
        }

    }  // namespace internal

    //////////////////////////////
    // Start of public API
    //////////////////////////////

    // * `cleantype::lambda_clean<typename... Args, typename Lambda>(Lambda fn)` is a function that
    // will return a string containing
    //    the readable signature of a lambda
    //
    // In the case of a generic lambda, you will need to specify the type of the auto parameters:
    // Example:
    //    auto f = [](auto x, auto y) { return x + y; };
    //    std::cout << cleantype::lambda_clean<int, char>(f) << std::endl;
    //     ==>   lambda: (int, char) -> int
    template <typename... Args, typename Lambda>
    std::string lambda_clean(Lambda fn)
    {
        // return cleantype::lambda(fn, true); // Won't compile : clang gives up here
        bool flag_clean = true;
        std::string memfn_type = cleantype::internal::lambda_memfn_type<Args...>(fn);
        std::string final_type =
            cleantype::internal::_mem_fn_to_lambda_type(memfn_type, flag_clean);
        return final_type;
    }

    // * `cleantype::lambda_clean<typename... Args, typename Lambda>(Lambda fn)` is a function that
    // will return a string containing
    //    the full signature of a lambda
    // In the case of a generic lambda, you will need to specify the type of the auto parameters
    // (see lambda_clean doc for an example)
    template <typename... Args, typename Lambda>
    std::string lambda_full(Lambda fn)
    {
        // return cleantype::lambda(fn, false); // Won't compile : clang gives up here
        bool flag_clean = false;
        std::string memfn_type = cleantype::internal::lambda_memfn_type<Args...>(fn);
        std::string final_type =
            cleantype::internal::_mem_fn_to_lambda_type(memfn_type, flag_clean);
        return final_type;
    }

    // * `cleantype::lambda<typename... Args, typename Lambda>(Lambda fn, bool flag_clean)` is a
    // function that will return
    //    a string containing the signature of a lambda.
    //    flag_clean controls wether the signature is cleaned or not.
    //
    // In the case of a generic lambda, you will need to specify the type of the auto parameters:
    // Example:
    //    auto f = [](auto x, auto y) { return x + y; };
    //    std::cout << cleantype::lambda<int, char>(f, true) << std::endl;
    //     ==>   lambda: (int, char) -> int
    template <typename... Args, typename Lambda>
    std::string lambda(Lambda fn, bool flag_clean)
    {
        std::string memfn_type = cleantype::internal::lambda_memfn_type<Args...>(fn);
        std::string final_type =
            cleantype::internal::_mem_fn_to_lambda_type(memfn_type, flag_clean);
        return final_type;
    }

}  // namespace cleantype

#define CT_show_details_lambda(f) std::string("[") + cleantype::lambda_clean(f) + "] " + #f
#define CT_show_details_lambda_full(f) std::string("[") + cleantype::lambda_full(f) + "] " + #f

// CT_type_lambda_generic_fromparams_
#define CT_type_lambda_generic_fromparams_1(fn, arg1) cleantype::lambda_clean<decltype(arg1)>(fn)
#define CT_type_lambda_generic_fromparams_2(fn, arg1, arg2) \
    cleantype::lambda_clean<decltype(arg1), decltype(arg2)>(fn)
#define CT_type_lambda_generic_fromparams_3(fn, arg1, arg2, arg3) \
    cleantype::lambda_clean<decltype(arg1), decltype(arg2), decltype(arg3)>(fn)
#define CT_type_lambda_generic_fromparams_4(fn, arg1, arg2, arg3, arg4) \
    cleantype::lambda_clean<decltype(arg1), decltype(arg2), decltype(arg3), decltype(arg4)>(fn)
#define CT_type_lambda_generic_fromparams_5(fn, arg1, arg2, arg3, arg4, arg5) \
    cleantype::lambda_clean<decltype(arg1),                                   \
                            decltype(arg2),                                   \
                            decltype(arg3),                                   \
                            decltype(arg4),                                   \
                            decltype(arg5)>(fn)


namespace cleantype
{
    /////////////////////////
    // readable types
    /////////////////////////
    // * `cleantype::clean<T...>()` is a function that will return a string containing
    //    a readable type, for a given type or pack of types
    //    Use it with `cleantype::clean<decltype(var)>()`
    //    Note:
    //      * It will add a reference by default so that
    //         int v = 5; cleantype::clean(v) will return "int&"
    //         => use the macro CT_cleantype_clean() if you want to avoid this

    // * `cleantype::clean<T...>(t...)` is a an easier version, using an instance of the type.
    //    Notes:
    //      * It will add a reference by default so that
    //         int v = 5; cleantype::clean(v) will return "int&"
    //         => use the macro CT_cleantype_clean() if you want to avoid this
    //      * It is not able to output correctly r-value references
    //          For this, use `cleantype::clean<decltype(var)>()`

    // * `cleantype::show_details(T && v)` is a function that will return a string containing
    //    the readable type of a variable, as well as its content

    // * `CT_cleantype_clean(var)` is a macro that will also return the full type,
    //    but, it is able to also correctly display rvalue reference types.

    // *  `CT_show_details(var)` is a macro that will return a string containing the name,
    //    type and content of a variable (in this case, the underlying type of 'var'
    //    has to have an 'ostream & operator<<')

    // * `CT_show_details_cont` (macro) is a version of CT_show_details for complex containers
    //    like "std::map". "cont" stands for "container".

    // * `cleantype::clean_typestring(type_names)` will clean a given (list of) type(s) given as a string
    // * `cleantype::indent_type_tree(type_names)` will present an indented view of a given (list of) type(s).

    /////////////////////////
    // full types
    /////////////////////////
    // * `cleantype::full<T...>()` is a function that will return a string containing
    //    the full type. It also works with packs of types.
    //    Use it with `cleantype::full<decltype(var)>()`
    //    Note:
    //      * It will add a reference by default so that
    //         int v = 5; cleantype::full(v) will return "int&"
    //         => use the macro CT_cleantype_full() if you want to avoid this

    // * `cleantype::full<T...>(t...)` is a an easier version, using an instance of the type.
    //    Notes:
    //      * It will add a reference by default so that
    //         int v = 5; cleantype::full(v) will return "int&"
    //         => use the macro CT_cleantype_full() if you want to avoid this
    //      * It is not able to output correctly r-value references
    //          For this, use `cleantype::full<decltype(var)>()`

    // * `cleantype::show_details_full(T && v)` is a function that will return a string containing
    //    the full type of a variable, as well as its content

    // * `CT_cleantype_full(var)` is a macro that will also return the full type,
    //    but, it is able to also correctly display rvalue reference types.

    // *  `CT_show_details_full(var)` is a macro that will return a string containing the name,
    //    type and content of a variable (in this case, the underlying type of 'var'
    //    has to have an 'ostream & operator<<')

    // * `CT_show_details_full_cont` is a version of CT_show_details_full for complex containers
    //    like "std::map". "cont" stands for "container".


    /////////////////////////////////////////
    // decipher type in your compiler output
    /////////////////////////////////////////
    // `ct_compiler_decipher` simplifies the template noise in your compiler output :
    // just "|" (pipe) your build tool to it.
    // Compile it with the following command:
    // $CC -Isrc/include --std=c++14 src/tools/ct_compiler_decipher/ct_compiler_decipher.cpp -o ct_compiler_decipher

    /////////////////////////////////////////
    // identify types in the compiler output
    /////////////////////////////////////////
    // * `CT_compiler_log_var_type(var)` is a macro that will create
    //   an intentional compile error, in which you will be able to read
    //   the variable type.
    // * `CT_compiler_log_type(var)` is a macro that will create
    //   an intentional compile error, in which you will be able to read
    //   the type definition
    // You can use these macros in combinaation with `ct_compiler_decipher`
    // (pipe your build output to `ct_compiler_decipher` in order to get
    //  readable types)

    /////////////////////////////////////////
    // Compile time constexpr type names
    /////////////////////////////////////////
    // cleantype::full_compiletime<T>() will give you the full name of a type in the form of a Boost.Hana string.
    // Note: For this, you need to manually include "cleantype/cleantype_compiler_typename.hpp",
    // and to have boost in your include path.

    ///////////////////////////////////////////////////////////
    // Identify the auto return type of functions and functors
    ///////////////////////////////////////////////////////////
    // * cleantype::invoke_result<F, Args...>::type and cleantype::invoke_result_t<F, Args...>
    //   will contain the type of any function / lambda / functor. This is a type, not a string.
    // * CT_invoke_result_fn(F, Args...) and CT_invoke_result_fn_template(F, Args...)
    //   are macros that makes it easy to get the return type of a function whose return type
    //   is marked as "auto" (the second version is for template functions)
    // * CT_type_fn, CT_type_fn_template, CT_type_fn_full and CT_type_fn_template_full
    //   are macros that return a string containing the return type of a function
    //   whose return type is marked as "auto"

    /////////////////////////
    // lambdas
    /////////////////////////
    // * `cleantype::lambda_clean<typename... Args, typename Lambda>(Lambda fn)` is a function that
    // will return a string containing
    //    the readable signature of a lambda
    //
    // In the case of a generic lambda, you will need to specify the type of the auto parameters:
    // Example:
    //    auto f = [](auto x, auto y) { return x + y; };
    //    std::cout << cleantype::lambda_clean<int, char>(f) << std::endl;
    //     ==>   lambda: (int, char) -> int

    // * `cleantype::lambda_clean<typename... Args, typename Lambda>(Lambda fn)` is a function that
    // will return a string containing
    //    the full signature of a lambda

    // * `cleantype::lambda<typename... Args, typename Lambda>(Lambda fn, bool flag_clean)` is a
    // function that will return
    //    a string containing the signature of a lambda.
    //    flag_clean controls wether the signature is cleaned or not.
    //
    // In the case of a generic lambda, you will need to specify the type of the auto parameters:
    // Example:
    //    auto f = [](auto x, auto y) { return x + y; };
    //    std::cout << cleantype::lambda<int, char>(f, true) << std::endl;
    //     ==>   lambda: (int, char) -> int

    // *  `CT_show_details_lambda(var)` is a macro that will return a string containing the
    //    readable signature of a lambda and its name

    // *  `CT_show_details_lambda_full(var)` is a macro that will return a string containing the
    //    full signature of a lambda and its name

    // *  `CT_type_lambda_generic_fromparams_XXX(lambda, arg1, arg2, ...)` is a macro that will
    // return a string containing the
    //     signature of a generic lambda where you do not specify the args type, instead you give
    //     example of these types. (XXX is the number of params of the lambda, and can vary from 1
    //     to 5).


    /////////////////////////////////////////
    // Settings - configure replacements and indentation
    /////////////////////////////////////////
    // In order to configure the behavior of cleantype:
    // Duplicate the file .cleantype.json at the root of the cleantype repository,
    // and place it wherever in the hierarchy of the parent directories of your application.
    // Edit this file as a normal json file

}  // namespace cleantype
