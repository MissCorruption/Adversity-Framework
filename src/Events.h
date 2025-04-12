#pragma once

#include "Event.h"
#include "Contexts.h"

namespace Adversity
{
    class Events
    {
    public:
        static void Load(const std::string& a_context, const std::string& a_pack, std::vector<Event>& a_events, ConditionParser::RefMap& a_refs);
        static Event* GetById(const std::string& a_id);
        static std::vector<Event*> GetByIds(const std::vector<std::string>& a_ids);
        static std::vector<Event*> GetInContext(const std::string& a_context);
        static std::vector<Event*> GetInPack(const std::string& a_pack);
        static std::vector<std::string> GetIds(const std::vector<Event*>& a_events);
        static std::vector<Event*> Filter(std::function<bool(Event* a_event)> a_check);
        static std::vector<Event*> Filter(const std::vector<Event*>& a_events, std::function<bool(Event* a_event)> a_check);

        static void PersistAll();
        static void Save(SKSE::SerializationInterface* a_intfc);
        static void Load(SKSE::SerializationInterface* a_intfc);
        static void Revert();

        template <typename T>
        static T GetValue(const std::string& a_id, const std::string& a_key, T a_default, bool a_persist);

        template <typename T>
        static bool SetValue(const std::string& a_id, const std::string& a_key, T a_value, bool a_persist);

    private:
        static inline std::unordered_map<std::string, Event> _events;
        static inline std::unordered_map<std::string, Meta> _persistent;
        static inline std::unordered_map<std::string, bool> _dirty;
        static inline std::unordered_map<std::string, Meta> _runtime;
        static inline std::unordered_map<std::string, std::vector<Event*>> _contexts;
        static inline std::unordered_map<std::string, std::vector<Event*>> _packs;
    };
}
