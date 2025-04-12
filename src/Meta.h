#pragma once

#include "Util.h"
#include "Serialization.h"

namespace Adversity
{
	using GenericData = std::variant<
		std::monostate,
		bool,
		int,
		float,
		std::string,
		RE::TESForm*,
		std::vector<bool>,
		std::vector<int>,
		std::vector<float>,
		std::vector<std::string>,
		std::vector<RE::TESForm*>>;

	enum DataType
	{
		None,
		Bool,
		Int,
		Float,
		String,
		Form,
		BoolList,
		IntList,
		FloatList,
		StringList,
		FormList
	};

	class Meta
	{
	public:
		Meta() = default;
		Meta(SKSE::SerializationInterface* a_intfc)
		{
			auto i = Serialization::Read<std::size_t>(a_intfc);
			_data.reserve(i);
			for (; i > 0; i--) {
				const auto key = Serialization::Read<std::string>(a_intfc);
				const auto index = Serialization::Read<std::size_t>(a_intfc);

				GenericData value;

				switch (index) {
					case DataType::Bool:
					{
						value = Serialization::Read<bool>(a_intfc);
						break;
					}
					case DataType::Int:
					{
						value = Serialization::Read<int>(a_intfc);
						break;
					}
					case DataType::Float:
					{
						value = Serialization::Read<float>(a_intfc);
						break;
					}
					case DataType::None:
					case DataType::String:
					{
						value = Serialization::Read<std::string>(a_intfc);
						break;
					}
					case DataType::Form:
					{
						value = Serialization::Read<RE::TESForm*>(a_intfc);
						break;
					}
					case DataType::BoolList:
					{
						value = Serialization::Read<std::vector<bool>>(a_intfc);
						break;
					}
					case DataType::IntList:
					{
						value = Serialization::Read<std::vector<int>>(a_intfc);
						break;
					}
					case DataType::FloatList:
					{
						value = Serialization::Read<std::vector<float>>(a_intfc);
						break;
					}
					case DataType::StringList:
					{
						value = Serialization::Read<std::vector<std::string>>(a_intfc);
						break;
					}
					case DataType::FormList:
					{
						value = Serialization::Read<std::vector<RE::TESForm*>>(a_intfc);
						break;
					}
					default:
					{
						logger::error("unrecognized type: {}", key);
						break;
					}
				}
			}
		}

		inline void Serialize(SKSE::SerializationInterface* a_intfc) const
		{
			Serialization::Write(a_intfc, _data.size());
			for (const auto& [key, value] : _data) {
				Serialization::Write(a_intfc, key);

				const auto& index = (DataType)value.index();

				Serialization::Write(a_intfc, value.index());

				switch (index)
				{
					case DataType::Bool: 
					{
						Serialization::Write(a_intfc, std::get<bool>(value));
						break;
					}
					case DataType::Int:
					{
						Serialization::Write(a_intfc, std::get<int>(value));
						break;
					}
					case DataType::Float:
					{
						Serialization::Write(a_intfc, std::get<float>(value));
						break;
					}
					case DataType::None:
					case DataType::String:
					{
						Serialization::Write(a_intfc, std::get<std::string>(value));
						break;
					}
					case DataType::Form:
					{
						Serialization::Write(a_intfc, std::get<RE::TESForm*>(value));
						break;
					}
					case DataType::BoolList:
					{
						Serialization::Write(a_intfc, std::get<std::vector<bool>>(value));
						break;
					}
					case DataType::IntList:
					{
						Serialization::Write(a_intfc, std::get<std::vector<int>>(value));
						break;
					}
					case DataType::FloatList:
					{
						Serialization::Write(a_intfc, std::get<std::vector<float>>(value));
						break;
					}
					case DataType::StringList:
					{
						Serialization::Write(a_intfc, std::get<std::vector<std::string>>(value));
						break;
					}
					case DataType::FormList:
					{
						Serialization::Write(a_intfc, std::get<std::vector<RE::TESForm*>>(value));
						break;
					}
					default:
					{
						logger::error("unrecognized type: {}", key);
						break;
					}
				}
			}
		}

		inline void Read(const YAML::Node& a_node)
		{
			for (YAML::const_iterator it = a_node.begin(); it != a_node.end(); ++it) {
				const auto key = Utility::CastLower(it)->first.as<std::string>();

				if (it->second.IsSequence()) {
					_data[key] = ParseData(it->second.as<std::vector<std::string>>(std::vector<std::string>{}));
				} else {
					_data[key] = ParseData(it->second.as<std::string>(""));
				}
			}
		}

		inline YAML::Node Write() const
		{
			YAML::Node node;

			// TODO: reimplement
			for (const auto& [key, value] : _data) {
				const auto& index = value.index();
				switch (index) {
					case DataType::None:
					{
						node[key] = "";
						break;	
					}
					case DataType::Bool:
					{
						node[key] = std::get<bool>(value);
						break;
					}
					case DataType::Int:
					{
						node[key] = std::get<int>(value);
						break;
					}
					case DataType::Float:
					{
						node[key] = std::get<float>(value);
						break;
					}
					case DataType::String:
					{
						node[key] = std::get<std::string>(value);
						break;
					}
					case DataType::Form:
					{
						node[key] = Utility::FormToString(std::get<RE::TESForm*>(value));
						break;
					}
					case DataType::BoolList:
					{
						node[key] = std::get<std::vector<bool>>(value);
						break;
					}
					case DataType::IntList:
					{
						node[key] = std::get<std::vector<int>>(value);
						break;
					}
					case DataType::FloatList:
					{
						node[key] = std::get<std::vector<float>>(value);
						break;
					}
					case DataType::StringList:
					{
						node[key] = std::get<std::vector<std::string>>(value);
						break;
					}
					case DataType::FormList:
					{
						const auto& forms = std::get<std::vector<RE::TESForm*>>(value);
						std::vector<std::string> values;
						for (const auto& form : forms) {
							values.push_back(form ? Utility::FormToString(form) : "none");
						}

						node[key] = values;
						break;
					}
					default:
					{
						logger::error("unrecognized type: {}", key);
						break;
					}
				}
			}


			return node;
		}

		template<typename T>
		inline bool HasValue(const std::string& a_key) const
		{
			const auto key{ Utility::CastLower(a_key) };

			const auto iter = _data.find(key);
			if (iter == _data.end()) {
				return false;
			}

			const auto& value = iter->second;
			if (std::holds_alternative<T>(value)) {
				return true;
			}

			return false;
		}

		template <typename T>
		inline T GetValue(const std::string& a_key, T a_default) const
		{
			const auto key{ Utility::CastLower(a_key) };

			const auto iter = _data.find(key);
			if (iter == _data.end()) {
				return a_default;
			}

			const auto& value = iter->second;
			if (std::holds_alternative<T>(value)) {
				return std::get<T>(value);
			}

			return a_default;
		}

		template <typename T>
		inline void SetValue(const std::string& a_key, T a_value)
		{
			const auto key{ Utility::CastLower(a_key) };
			GenericData value{ a_value };
			_data[key] = value;
		}
	private:
		static GenericData ConvertToGeneric(const std::string& a_str)
		{
			GenericData value = a_str;
			if ((Utility::CastLower(a_str)) == "none") {
				value = nullptr;
			} else if (const auto form = Utility::FormFromString<RE::TESForm*>(a_str)) {
				value = form;
			} else if (a_str == "true" || a_str == "false") {
				value = (bool)(a_str == "true");
			} else if (Utility::IsNumericString(a_str)) {
				value = a_str.contains('.') ? (float)std::stof(a_str) : (int)std::stoi(a_str);
			}

			return value;
		}

		static inline GenericData ParseData(std::string a_raw)
		{
			return ConvertToGeneric(a_raw);
		}

		static inline GenericData ParseData(std::vector<std::string> a_raw)
		{
			GenericData values;

			if (a_raw.empty()) {
				values = std::vector<std::string>{};
				return values;
			}

			auto index = ConvertToGeneric(a_raw[0]).index();
			bool valid = true;

			std::vector<GenericData> convertedValues;
			convertedValues.reserve(a_raw.size());

			for (const auto& val : a_raw) {
				const auto converted = ConvertToGeneric(val);
				if (index != converted.index()) {
					valid = false;
				}
				convertedValues.push_back(converted);
			}


			if (!valid) {
				index = DataType::String;
			}


			switch (index) {
			case DataType::Bool:
				values = CreateList<bool>(convertedValues);
				break;
			case DataType::Int:
				values = CreateList<int>(convertedValues);
				break;
			case DataType::Float:
				values = CreateList<float>(convertedValues);
				break;
			case DataType::String:
				values = CreateList<std::string>(convertedValues);
				break;
			case DataType::Form:
				values = CreateList<RE::TESForm*>(convertedValues);
				break;
			}

			return values;
		}

		template <typename T>
		static inline std::vector<T> CreateList(std::vector<GenericData>& a_values)
		{
			std::vector<T> converted;
			converted.reserve(a_values.size());
			for (const auto& val : a_values) {
				converted.push_back(std::get<T>(val));
			}
			return converted;
		}

		std::unordered_map<std::string, GenericData> _data;
		friend struct YAML::convert<Meta>;
	};
}

namespace YAML
{
	using namespace Adversity;

	template <>
	struct convert<Meta>
	{
		static bool decode(const Node& node, Meta& rhs)
		{
			rhs.Read(node);
			return true;
		}

		static Node encode(const Meta& rhs)
		{
			return rhs.Write();
		}
	};
}