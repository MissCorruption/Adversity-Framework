#include "ConditionParser.h"
#include "EnumLookup.h"

using namespace Conditions;

auto ConditionParser::Parse(std::string_view a_text, const RefMap& a_refs) -> RE::TESConditionItem*
{
    std::string textStr = std::string(a_text);
    const auto splits = Utility::StringSplit(textStr, "<>"sv)
                        | std::ranges::views::transform([](std::string& a_str) {
                            a_str.erase(
                                std::remove_if(a_str.begin(), a_str.end(), ::isspace),
                                a_str.end()
                            );
                            return a_str;
                        })
                        | std::ranges::to<std::vector<std::string>>(); // Use std::string

    const std::string text = splits.size() == 2 ? splits[1] : splits[0];
    const std::string refStr = splits.size() == 2 ? splits[0] : "";

    static srell::regex re{
        R"((\w+)\s+((\w+)(\s+([\w:]+))?\s*)?(==|!=|>|>=|<|<=)\s*(\w+)(\s+(AND|OR))?)"
    };

    srell::cmatch m;
    if (!srell::regex_match(text.data(), m, re)) {
        logger::error("Could not parse condition: {}", a_text);
        return nullptr;
    }

    RE::CONDITION_ITEM_DATA data;
    auto& mFunction = m[1];
    auto& mParam1 = m[3];
    auto& mParam2 = m[5];
    auto& mOperator = m[6];
    auto& mComparand = m[7];
    auto& mConnective = m[9];

    auto function = RE::SCRIPT_FUNCTION::LocateScriptCommand(mFunction.str().data());

    if (!function || !function->conditionFunction) {
        logger::error("Did not find condition function: {}", mFunction.str());
        return nullptr;
    }

    constexpr auto FUNCTION_INDEX_OFFSET = 0x1000;
    auto functionIndex = std::to_underlying(function->output) - FUNCTION_INDEX_OFFSET;
    data.functionData.function = static_cast<RE::FUNCTION_DATA::FunctionID>(functionIndex);

    if (mParam1.matched) {
        if (function->numParams >= 1) {
            data.functionData.params[0] = std::bit_cast<void*>(
                ParseParam(mParam1.str(), function->params[0].paramType.get(), a_refs));
        } else {
            logger::warn("Condition function {} ignoring parameter: {}", function->functionName, mParam1.str());
        }
    }

    if (mParam2.matched) {
        if (function->numParams >= 2) {
            data.functionData.params[1] = std::bit_cast<void*>(
                ParseParam(mParam2.str(), function->params[1].paramType.get(), a_refs));
        } else {
            logger::warn("Condition function {} ignoring parameter: {}", function->functionName, mParam2.str());
        }
    }

    if (mOperator.matched) {
        auto op = mOperator.str();
        if (op == "==") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kEqualTo;
        } else if (op == "!=") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kNotEqualTo;
        } else if (op == ">") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kGreaterThan;
        } else if (op == ">=") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kGreaterThanOrEqualTo;
        } else if (op == "<") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kLessThan;
        } else if (op == "<=") {
            data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kLessThanOrEqualTo;
        }
    } else {
        data.flags.opCode = RE::CONDITION_ITEM_DATA::OpCode::kNotEqualTo;
    }

    if (mComparand.matched) {
        auto comparand = mComparand.str();
        if (auto global = RE::TESForm::LookupByEditorID<RE::TESGlobal>(comparand)) {
            data.comparisonValue.g = global;
            data.flags.global = true;
        } else {
            data.comparisonValue.f = std::stof(comparand);
        }
    } else {
        data.comparisonValue.f = 0.0f;
    }

    if (mConnective.matched) {
        auto connective = mConnective.str();
        if (connective == "OR") {
            data.flags.isOR = true;
        }
    }

    if (!refStr.empty()) {
        if (const auto ref = LookupForm<RE::TESObjectREFR>(refStr, a_refs)) {
            data.runOnRef = ref->CreateRefHandle();
            data.object = RE::CONDITIONITEMOBJECT::kRef;
        } else {
            logger::info("Failed to find ref {}", refStr);
        }
    }

    auto conditionItem = new RE::TESConditionItem();
    conditionItem->data = data;
    return conditionItem;
}

auto ConditionParser::ParseParam(
    const std::string& a_text,
    RE::SCRIPT_PARAM_TYPE a_type,
    const RefMap& a_refs) -> ConditionParam
{
    ConditionParam param{};

    auto textCIS = Utility::ToUpper(a_text);

    switch (a_type) {
    case RE::SCRIPT_PARAM_TYPE::kChar:
    case RE::SCRIPT_PARAM_TYPE::kInt:
    case RE::SCRIPT_PARAM_TYPE::kStage:
    case RE::SCRIPT_PARAM_TYPE::kRelationshipRank:
        param.i = std::stoi(textCIS);
        break;
    case RE::SCRIPT_PARAM_TYPE::kFloat:
        param.f = std::stof(textCIS);
        break;
    case RE::SCRIPT_PARAM_TYPE::kActorValue:
        param.i = std::to_underlying(EnumLookup::LookupActorValue(textCIS));
        break;
    case RE::SCRIPT_PARAM_TYPE::kAxis:
        param.i = EnumLookup::LookupAxis(textCIS);
        break;
    case RE::SCRIPT_PARAM_TYPE::kSex:
        param.i = EnumLookup::LookupSex(textCIS);
        break;
    case RE::SCRIPT_PARAM_TYPE::kCastingSource:
        param.i = std::to_underlying(EnumLookup::LookupCastingSource(textCIS));
        break;
    case RE::SCRIPT_PARAM_TYPE::kVMScriptVar:
        param.str = new RE::BSString(a_text.c_str());
        break;
    default:
        param.form = LookupForm(textCIS, a_refs);
        break;
    }

    return param;
}
