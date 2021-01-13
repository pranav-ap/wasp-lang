#pragma once
#include "Objects.h"
#include <json.hpp>

void to_json(nlohmann::json& j, const Object_ptr obj);
void from_json(const nlohmann::json& j, Object_ptr obj);

void to_json(nlohmann::json& j, const Object& o);
void from_json(const nlohmann::json& j, Object& o);

void to_json(nlohmann::json& j, const IntObject& o);
void from_json(const nlohmann::json& j, IntObject& o);

void to_json(nlohmann::json& j, const FloatObject& o);
void from_json(const nlohmann::json& j, FloatObject& o);

void to_json(nlohmann::json& j, const StringObject& o);
void from_json(const nlohmann::json& j, StringObject& o);

void to_json(nlohmann::json& j, const BooleanObject& o);
void from_json(const nlohmann::json& j, BooleanObject& o);

void to_json(nlohmann::json& j, const NoneObject& o);
void from_json(const nlohmann::json& j, NoneObject& o);

void to_json(nlohmann::json& j, const ListObject& o);
void from_json(const nlohmann::json& j, ListObject& o);

void to_json(nlohmann::json& j, const TupleObject& o);
void from_json(const nlohmann::json& j, TupleObject& o);

void to_json(nlohmann::json& j, const SetObject& o);
void from_json(const nlohmann::json& j, SetObject& o);

void to_json(nlohmann::json& j, const MapObject& o);
void from_json(const nlohmann::json& j, MapObject& o);

void to_json(nlohmann::json& j, const VariantObject& o);
void from_json(const nlohmann::json& j, VariantObject& o);

void to_json(nlohmann::json& j, const ReturnObject& o);
void from_json(const nlohmann::json& j, ReturnObject& o);

void to_json(nlohmann::json& j, const ErrorObject& o);
void from_json(const nlohmann::json& j, ErrorObject& o);

void to_json(nlohmann::json& j, const YieldObject& o);
void from_json(const nlohmann::json& j, YieldObject& o);

void to_json(nlohmann::json& j, const RedoObject& o);
void from_json(const nlohmann::json& j, RedoObject& o);

void to_json(nlohmann::json& j, const BreakObject& o);
void from_json(const nlohmann::json& j, BreakObject& o);

void to_json(nlohmann::json& j, const ContinueObject& o);
void from_json(const nlohmann::json& j, ContinueObject& o);

void to_json(nlohmann::json& j, const BuiltInObject& o);
void from_json(const nlohmann::json& j, BuiltInObject& o);

void to_json(nlohmann::json& j, const AnyType& o);
void from_json(const nlohmann::json& j, AnyType& o);

void to_json(nlohmann::json& j, const IntLiteralType& o);
void from_json(const nlohmann::json& j, IntLiteralType& o);

void to_json(nlohmann::json& j, const FloatLiteralType& o);
void from_json(const nlohmann::json& j, FloatLiteralType& o);

void to_json(nlohmann::json& j, const StringLiteralType& o);
void from_json(const nlohmann::json& j, StringLiteralType& o);

void to_json(nlohmann::json& j, const BooleanLiteralType& o);
void from_json(const nlohmann::json& j, BooleanLiteralType& o);

void to_json(nlohmann::json& j, const IntType& o);
void from_json(const nlohmann::json& j, IntType& o);

void to_json(nlohmann::json& j, const FloatType& o);
void from_json(const nlohmann::json& j, FloatType& o);

void to_json(nlohmann::json& j, const StringType& o);
void from_json(const nlohmann::json& j, StringType& o);

void to_json(nlohmann::json& j, const BooleanType& o);
void from_json(const nlohmann::json& j, BooleanType& o);

void to_json(nlohmann::json& j, const ListType& o);
void from_json(const nlohmann::json& j, ListType& o);

void to_json(nlohmann::json& j, const TupleType& o);
void from_json(const nlohmann::json& j, TupleType& o);

void to_json(nlohmann::json& j, const SetType& o);
void from_json(const nlohmann::json& j, SetType& o);

void to_json(nlohmann::json& j, const MapType& o);
void from_json(const nlohmann::json& j, MapType& o);

void to_json(nlohmann::json& j, const VariantType& o);
void from_json(const nlohmann::json& j, VariantType& o);

void to_json(nlohmann::json& j, const NoneType& o);
void from_json(const nlohmann::json& j, NoneType& o);
