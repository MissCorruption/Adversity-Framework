#include "Tattoos.h"
#include "Util.h"

using namespace Adversity;

void Tattoos::Load(const std::string& a_context, const std::string& a_pack)
{
    Util::ProcessEntities<std::vector<Group>>(a_context, a_pack, "tattoos", [](const std::string& a_id, std::vector<Group> a_group) {
        _groups.insert({ a_id, std::move(a_group) });
    });
}

std::vector<Group*> Tattoos::GetGroups(const std::string& a_context, const std::string& a_name)
{
    std::vector<Group*> groups;
    const auto id = std::format("{}/{}", a_context, Utility::CastLower(a_name));
    if (!_groups.count(id)) {
        return groups;
    }

    auto& found = _groups.at(id);
    groups.reserve(found.size());
    for (auto& group : found) {
        groups.push_back(&group); // Ensure 'group' is not const
    }

    return groups;
}
