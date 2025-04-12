#pragma once

#include <atomic>
#include <format>
#include <functional>
#include <locale>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <ranges>
#include <filesystem>
#include <vector>
#include <variant>

#pragma warning(push)
#pragma warning(disable: 4702)
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#pragma warning(pop)

#pragma warning(push)
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#pragma warning(pop)

#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <srell.hpp>
#include <yaml-cpp/yaml.h>

// Project-Specific Headers
#include "Util/FormLookup.h"
#include "Util/Random.h"
#include "Util/StringUtil.h"
#include "Util/Script.h"

// Static Assertions
static_assert(magic_enum::is_magic_enum_supported, "magic_enum is not supported");

// Namespace Aliases
using json = nlohmann::json;
namespace logger = SKSE::log;
namespace fs = std::filesystem;
using namespace std::literals;

// Macro Definitions
#define REL_ID(se, ae) REL::RelocationID(se, ae)
#define REL_OF(se, ae, vr) REL::VariantOffset(se, ae, vr)

namespace stl
{
    using namespace SKSE::stl;

    template <class T>
    void write_thunk_call(std::uintptr_t a_src)
    {
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        T::func = trampoline.write_call<5>(a_src, T::thunk);
    }

    template <class T>
    void write_thunk_call_6(std::uintptr_t a_src)
    {
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        T::func = *(uintptr_t*)trampoline.write_call<6>(a_src, T::thunk);
    }

    template <class F, size_t index, class T>
    void write_vfunc()
    {
        REL::Relocation<std::uintptr_t> vtbl{ F::VTABLE[index] };
        T::func = vtbl.write_vfunc(T::size, T::thunk);
    }

    template <std::size_t idx, class T>
    void write_vfunc(REL::VariantID id)
    {
        REL::Relocation<std::uintptr_t> vtbl{ id };
        T::func = vtbl.write_vfunc(idx, T::thunk);
    }

    template <class T>
    void write_thunk_jmp(std::uintptr_t a_src)
    {
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        T::func = trampoline.write_branch<5>(a_src, T::thunk);
    }

    template <class F, class T>
    void write_vfunc()
    {
        write_vfunc<F, 0, T>();
    }
}

namespace Adversity::Papyrus
{
    // Macros for Context, Event, and Actor Configuration
    #define CONTEXTCONFIG(name, type)                                                                                           \
        inline type GetContext##name(RE::StaticFunctionTag*, std::string a_id, std::string a_key, type a_default, bool a_persist) \
        {                                                                                                                         \
            return Contexts::GetValue<type>(a_id, a_key, a_default, a_persist);                                                     \
        }                                                                                                                         \
        inline bool SetContext##name(RE::StaticFunctionTag*, std::string a_id, std::string a_key, type a_val, bool a_persist)     \
        {                                                                                                                         \
            return Contexts::SetValue<type>(a_id, a_key, a_val, a_persist);                                                         \
        }

    #define EVENTCONFIG(name, type)                                                                                           \
        inline type GetEvent##name(RE::StaticFunctionTag*, std::string a_id, std::string a_key, type a_default, bool a_persist) \
        {                                                                                                                       \
            return Events::GetValue<type>(a_id, a_key, a_default, a_persist);                                                     \
        }                                                                                                                       \
        inline bool SetEvent##name(RE::StaticFunctionTag*, std::string a_id, std::string a_key, type a_val, bool a_persist)     \
        {                                                                                                                       \
            return Events::SetValue<type>(a_id, a_key, a_val, a_persist);                                                         \
        }

    #define ACTORCONFIG(name, type)                                                                                                    \
        inline type GetActor##name(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::string a_key, type a_default) \
        {                                                                                                                                \
            return Actors::GetValue<type>(a_context, a_actor, a_key, a_default);                                                           \
        }                                                                                                                                \
        inline bool SetActor##name(RE::StaticFunctionTag*, std::string a_context, RE::Actor* a_actor, std::string a_key, type a_val)     \
        {                                                                                                                                \
            return Actors::SetValue<type>(a_context, a_actor, a_key, a_val);                                                               \
        }

    // Macros for Registering Configuration Functions
    #define CONFIGFUNCS(configType)                                                    \
        configType(Bool, bool)                                                           \
        configType(Int, int)                                                             \
        configType(Float, float)                                                         \
        configType(String, std::string)                                                  \
        configType(Form, RE::TESForm*)                                                   \
        configType(BoolList, std::vector<bool>)                                          \
        configType(IntList, std::vector<int>)                                            \
        configType(FloatList, std::vector<float>)                                        \
        configType(StringList, std::vector<std::string>)                                 \
        configType(FormList, std::vector<RE::TESForm*>)

    #define REGISTERCONFIG(configType)                       \
        configType(Bool)                                       \
        configType(Int)                                        \
        configType(Float)                                      \
        configType(String)                                     \
        configType(Form)                                       \
        configType(BoolList)                                   \
        configType(IntList)                                    \
        configType(FloatList)                                  \
        configType(StringList)                                 \
        configType(FormList)

    #define REGISTERCONTEXT(name)    \
        REGISTERFUNC(GetContext##name) \
        REGISTERFUNC(SetContext##name)

    #define REGISTEREVENT(name)    \
        REGISTERFUNC(GetEvent##name) \
        REGISTERFUNC(SetEvent##name)

    #define REGISTERACTOR(name)    \
        REGISTERFUNC(GetActor##name) \
        REGISTERFUNC(SetActor##name)

    #define REGISTERFUNC(func) a_vm->RegisterFunction(#func##sv, "Adversity", func);
    #define REGISTERFUNCND(func) a_vm->RegisterFunction(#func##sv, "Adversity", func, true);

    using VM = RE::BSScript::IVirtualMachine;
    using StackID = RE::VMStackID;
}

// DLL Export Macro
#define DLLEXPORT __declspec(dllexport)
