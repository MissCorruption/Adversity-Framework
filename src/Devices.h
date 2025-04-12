#pragma once

namespace Adversity
{
	struct Device
	{
		std::string name;
		RE::FormID formId;
		std::string espName;
		RE::TESObjectARMO* armo;
	};

	inline void from_json(const json& j, Device& d)
	{
		j.at("name").get_to(d.name);
		j.at("espName").get_to(d.espName);

		auto formId = j.at("formId").template get<std::string>();
		d.formId = (int)strtol(formId.c_str(), NULL, 0);

		d.armo = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(d.formId, d.espName);
	}

	class Devices
	{
	public:
		static void Init();
		static void Load(const std::string& a_context);
		static std::vector<RE::TESObjectARMO*> GetDevicesByKeyword(std::string a_context, RE::Actor* a_actor, RE::BGSKeyword* a_kwd);
		static inline std::vector<RE::BGSKeyword*> GetExclKwds() { return _excludeKwds; }
		static bool DeviceMatches(std::string a_name, std::vector<std::string> a_filters);
	private:
		static inline std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Device>>> _devices;
		static inline std::unordered_map<std::string, std::vector<std::string>> _sets;
		static inline std::unordered_map<RE::FormID, Device*> _pieces;

		static inline std::vector<RE::BGSKeyword*> _excludeKwds;
		static inline RE::BGSKeyword* _lockableKwd;
		static inline RE::BGSKeyword* _deviceKwd;
	};
}

