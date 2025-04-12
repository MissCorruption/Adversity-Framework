#pragma once

namespace Adversity
{
    struct Tattoo
    {
        std::string section;
        std::string name;
    };

    struct Group
    {
        std::vector<Tattoo> tattoos;
        std::unordered_set<std::string> tags;
    };

    class Tattoos
    {
    public:
        static void Load(const std::string& a_context, const std::string& a_pack);
        static std::vector<Group*> GetGroups(const std::string& a_context, const std::string& a_name);

    private:
        static inline std::unordered_map<std::string, std::vector<Group>> _groups;
    };
}

namespace YAML
{
    using namespace Adversity;

    template <>
    struct convert<Group>
    {
        static bool decode(const Node& node, Group& rhs)
        {
            if (node["tattoos"] && node["tags"]) {
                rhs.tattoos = node["tattoos"].as<std::vector<Tattoo>>();
                const auto tags = node["tags"].as<std::vector<std::string>>(std::vector<std::string>{});
                rhs.tags = std::unordered_set<std::string>{ tags.begin(), tags.end() };
                return !rhs.tattoos.empty();
            }
            return false;
        }
    };

    template <>
    struct convert<Tattoo>
    {
        static bool decode(const Node& node, Tattoo& rhs)
        {
            if (node["name"] && node["section"]) {
                rhs.name = node["name"].as<std::string>();
                rhs.section = node["section"].as<std::string>();
                return !rhs.name.empty() && !rhs.section.empty();
            }
            return false;
        }
    };
}
