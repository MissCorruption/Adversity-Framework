#include "Packs.h"
#include "Events.h"
#include "Outfits.h"
#include "Tattoos.h"

using namespace Adversity;


void Packs::Load(std::string a_context)
{
	_contexts[a_context].clear();

	const std::string dir{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Packs", a_context) };
	for (const auto& a : fs::directory_iterator(dir)) {
		if (!fs::is_directory(a)) {
			continue;
		}

		const auto packName { Utility::CastLowera.path().filename().replace_extension().string()) };
		const auto id{ a_context + '/' + packName };
		
		try {
			const auto path{ a.path().string() + "/manifest.yaml" };

			auto config = YAML::LoadFile(path);

			logger::info("inserting {}", id);
			_packs.insert({ id, config.as<Pack>() });
			auto& pack = _packs[id];
			pack.Init(a_context, id);

			_contexts[a_context].push_back(&pack);

			auto refMap = ConditionParser::GenerateRefMap(config["refMap"].as<std::unordered_map<std::string, std::string>>(std::unordered_map<std::string, std::string>{}));

			Events::Load(a_context, id, pack.events, refMap);
			Outfits::Load(a_context, packName);
			Tattoos::Load(a_context, packName);

			logger::info("loaded pack {}", packName);
		} catch (std::exception& e) {
			logger::info("failed to load pack {} due to {}", id, e.what());
		} catch (...) {
			logger::info("failed to load pack {}", id);
		}
	}
	
}

void Packs::Reload(std::string a_context)
{
	for (const auto& pack : _contexts[a_context]) {
		const auto& packName = pack->GetName();
		Outfits::Load(a_context, packName);
		Tattoos::Load(a_context, packName);
	}
}


Pack* Packs::GetById(std::string a_pack) {
	const std::string id{ Utility::CastLowera_pack) };
	logger::info("Packs::GetById: {} - {}", a_pack, _packs.count(id));
	return _packs.count(id) ? &_packs[id] : nullptr;
}

std::vector<Pack*> Packs::GetByContext(std::string a_context)
{
	return _contexts[Utility::CastLowera_context)];
}

std::vector<std::string> Packs::GetIds(std::vector<Pack*> a_packs)
{
	std::vector<std::string> ids;
	for (auto pack : a_packs)
		ids.push_back(pack->GetId());
	return ids;
}