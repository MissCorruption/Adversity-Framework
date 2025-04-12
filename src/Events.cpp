#include "Events.h"
#include "Util.h"
#include "Serialization.h"

using namespace Adversity;

void Events::Load(const std::string& a_context, const std::string& a_pack, std::vector<Event>& a_events, ConditionParser::RefMap& a_refs)
{
    for (auto& event : a_events) {
        event.Init(a_context, a_pack, a_refs);
        _events[event.GetId()] = std::move(event);

        logger::info("loading event: {}", event.GetId());

        auto ref = GetById(event.GetId());
        _contexts[a_context].push_back(ref);
        _packs[a_pack].push_back(ref);
    }

    for (const auto& [context, _] : _contexts) {
        const std::string file{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Config/events.yaml", context) };

        if (fs::exists(file)) {
            const auto& config = YAML::Load(file).as<std::unordered_map<std::string, Meta>>();
            for (const auto& [id, data] : config) {
                _persistent[id] = data;
            }
        }
    }
}

void Events::PersistAll()
{
    for (const auto& [id, persist] : _dirty) {
        if (!persist)
            continue;

        std::unordered_map<std::string, Meta> data;
        for (const auto& ev : _contexts[id]) {
            if (_persistent.count(ev->GetId())) {
                data[ev->GetId()] = _persistent[ev->GetId()];
            }
        }

        YAML::Node node{ data };

        const std::string file{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Config/events.yaml", id) };
        std::ofstream fout(file);
        fout << node;
    }
}

void Events::Save(SKSE::SerializationInterface* a_intfc)
{
    Serialization::Write(a_intfc, _runtime.size());
    for (const auto& [id, data] : _runtime) {
        Serialization::Write(a_intfc, id);
        data.Serialize(a_intfc);
    }
}

void Events::Load(SKSE::SerializationInterface* a_intfc)
{
    auto size = Serialization::Read<std::size_t>(a_intfc);
    for (; size > 0; size--) {
        const auto id = Serialization::Read<std::string>(a_intfc);
        Meta meta{ a_intfc };
        _runtime[id] = std::move(meta);
    }
}

void Events::Revert()
{
    _runtime.clear();
}

Event* Events::GetById(const std::string& a_id)
{
    const std::string lower_id = Utility::CastLower(a_id);
    return _events.count(lower_id) ? &_events[lower_id] : nullptr;
}

std::vector<Event*> Events::GetByIds(const std::vector<std::string>& a_ids)
{
    std::vector<Event*> events;

    for (const auto& id : a_ids) {
        if (auto ev = GetById(id))
            events.push_back(ev);
    }

    return events;
}

std::vector<Event*> Events::GetInContext(const std::string& a_context)
{
    return _contexts[a_context];
}

std::vector<Event*> Events::GetInPack(const std::string& a_pack)
{
    return _packs[a_pack];
}

std::vector<std::string> Events::GetIds(const std::vector<Event*>& a_events)
{
    std::vector<std::string> ids;
    for (const auto ev : a_events) {
        ids.push_back(ev->GetId());
    }

    return ids;
}

std::vector<Event*> Events::Filter(std::function<bool(Event* a_event)> a_check)
{
    std::vector<Event*> filtered;

    for (auto& [_, event] : _events) {
        if (a_check(&event))
            filtered.push_back(&event);
    }

    return filtered;
}

std::vector<Event*> Events::Filter(const std::vector<Event*>& a_events, std::function<bool(Event* a_event)> a_check)
{
    std::vector<Event*> filtered;

    for (const auto& event : a_events) {
        if (a_check(event))
            filtered.push_back(event);
    }

    return filtered;
}
