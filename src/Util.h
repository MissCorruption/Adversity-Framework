#pragma once
#include "UI.h"


namespace fs = std::filesystem;

namespace Adversity
{
    class Util
    {
    public:
        template <typename T>
        static inline T* GetFormById(RE::FormID a_id)
        {
            return RE::TESDataHandler::GetSingleton()->LookupForm<T>(a_id, "Adversity Framework.esm");
        }

        template <typename T>
        static inline void ProcessEntities(std::string a_context, std::string a_pack, std::string a_type, std::function<void(std::string, T)> a_func)
        {
            const std::string dir{ std::format("data/skse/adversityframework/contexts/{}/packs/{}/{}", a_context, a_pack, a_type) };

            if (!fs::is_directory(dir)) {
                logger::warn("{}/{} has no {} directory", a_context, a_pack, a_type);
                return;
            }

            for (const auto& a : fs::directory_iterator(dir)) {
                if (fs::is_directory(a)) {
                    continue;
                }

                if (const auto ext = a.path().extension(); ext != ".yaml" && ext != ".yml") {
                    continue;
                }

                auto actual{ a.path().string() };

                if (actual.ends_with(".custom.yaml")) {
                    continue;
                }

                const auto filename{ a.path().filename().replace_extension().string() };

                try {
                    const auto custom{ std::format("{}.custom.yaml", actual.substr(0, actual.length() - 5)) };

                    const auto& path = fs::exists(custom) ? custom : actual;

                    auto config = YAML::LoadFile(path);

                    const std::string id{ std::format("{}/{}", a_context, Utility::CastLowerFilename(filename)) };
                    a_func(id, config.as<T>());
                    logger::info("loaded {} {} in {} successfully", a_type, filename, a_context);
                } catch (const std::exception& e) {
                    logger::error("failed to load {} {} in {}: {}", a_type, filename, a_context, e.what());
                } catch (...) {
                    logger::error("failed to load {} {} in {}", a_type, filename, a_context);
                }
            }
        }

        static float GetGameTime()
        {
            // TODO: Check if this is the correct global
            return RE::Calendar::GetSingleton()->GetCurrentGameTime();
            // return RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(0x39, "Skyrim.esm")->value;
        }

        static void AddKwd(RE::TESObjectARMO* a_form, std::string a_kwd)
        {
            if (const auto kwd = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(a_kwd)) {
                a_form->AddKeyword(kwd);
            }
        }
    };
}
