#pragma once
#include <SKSE/SKSE.h>
#include "Util.h"
#include "Util/FormLookup.h"

namespace Adversity::Serialization
{
	template <typename T>
	struct IsVector : std::false_type
	{};
	template <typename... P>
	struct IsVector<std::vector<P...>> : std::true_type
	{};

	template <typename T>
	inline T Read(SKSE::SerializationInterface* serde)
	{
		T val;
		serde->ReadRecordData(&val, sizeof(val));
		return val;
	}

	template <>
	inline std::string Read<std::string>(SKSE::SerializationInterface* serde)
	{
		std::size_t nameSize;
		serde->ReadRecordData(&nameSize, sizeof(nameSize));

		std::string name;
		name.reserve(nameSize);

		char c;
		for (int i = 0; i < nameSize; i++) {
			serde->ReadRecordData(&c, sizeof(c));
			name += c;
		}
		return name;
	}

	template <class T>
	inline void Read(SKSE::SerializationInterface* serde, std::vector<T>& a_values)
	{
		auto size = Read<std::size_t>(serde);
		for (; size > 0; size--) {
			a_values.push_back(Read<T>(serde));
		}
	}
	
	template <>
	inline RE::TESForm* Read<RE::TESForm*>(SKSE::SerializationInterface* serde)
	{
		return Utility::FormFromString<RE::TESForm*>(Read<std::string_view>(serde));
	}



	template <typename T>
	inline void Write(SKSE::SerializationInterface* serde, const T& value)
	{
		serde->WriteRecordData(&value, sizeof(value));
	}

	inline void Write(SKSE::SerializationInterface* serde, const std::string& name)
	{
		const auto& size = name.size();
		serde->WriteRecordData(&size, sizeof(size));

		char c;
		for (int i = 0; i < size; i++) {
			c = name[i];
			serde->WriteRecordData(&c, sizeof(c));
		}
	}

	template <typename T>
	inline void Write(SKSE::SerializationInterface* serde, const std::vector<T>& a_values)
	{
		const auto& size = a_values.size();
		serde->WriteRecordData(&size, sizeof(size));

		for (const auto& value : a_values) {
			Write<T>(serde, value);
		}
	}

	inline void Write(SKSE::SerializationInterface* serde, RE::TESForm* a_form)
	{
		Write(serde, Utility::FormToString(a_form));
	}
}