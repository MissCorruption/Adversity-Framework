#include "Util.h"
#include "Actors.h"

using namespace Adversity;

void Actors::Load(std::string a_context)
{
	_locks[a_context];
	_actors[a_context];

	const std::string base{ std::format("Data/SKSE/AdversityFramework/Contexts/{}/Actors", a_context) };
	const std::string actual{ base + "/data.yaml" };
	const std::string custom{ base + "/data.custom.yaml" };

	const auto& file = fs::exists(custom) ? custom : actual;

	if (!fs::exists(file)) {
		logger::info("{} - no actors config - skipping", a_context);
		return;
	}

	const auto actorFile = YAML::LoadFile(file);
	const auto actorData = actorFile.as<std::vector<Actor>>();
	for (const auto& actor : actorData) {
		if (const auto id = actor.GetId()) {
			_actors[a_context].insert({ id, actor });
		}
	}

	const std::string dir{ base + "/Traits" };
	for (const auto& a : fs::directory_iterator(dir)) {
		if (fs::is_directory(a)) {
			continue;
		}

		const auto id{ Utility::CastLower(a.path().filename().replace_extension().string()) };

		try {
			const auto path{ a.path().string() };
			auto config = YAML::LoadFile(path);
			auto trait = config.as<Trait>();
			trait.Init(a_context, id);

			_traits[a_context].insert({ trait.GetID(), trait });
		} catch (std::exception& e) {
			logger::info("failed to load actor data {}", id, e.what());
		} catch (...) {
			logger::info("failed to load actor data {}", id);
		}
	}
}