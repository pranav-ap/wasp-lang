//#pragma once
//
//#include "Evaluator.h"
//#include "Module.h"
//#include "TypeSystem.h"
//#include "ObjectSystem.h"
//#include "Builtins.h"
//#include "DispatchTables.h"
//#include "Assertion.h"
//
//#include <memory>
//#include <string>
//#include <map>
//#include <optional>
//#include <variant>
//
//#define MAKE_OBJECT_VARIANT(x) std::make_shared<Object>(x)
//#define MAKE_TYPE(x) std::make_shared<Type>(x)
//#define VOID std::make_shared<Object>(ReturnObject())
//#define NULL_CHECK(x) ASSERT(x != nullptr, "Oh shit! A nullptr")
//#define THROW(message) return std::make_shared<Object>(ErrorObject(message))
//#define THROW_ASSERT(condition, message) if (!condition) { return std::make_shared<Object>(ErrorObject(message)); }
//
//template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
//template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;
//
//using std::string;
//using std::map;
//using std::pair;
//using std::vector;
//using std::optional;
//using std::make_shared;
//using std::holds_alternative;
//using std::visit;
//using std::get;
//
//// API
//
//void Evaluator::execute(Module mod)
//{
//	for (auto statement : mod.nodes)
//	{
//		evaluate(statement);
//	}
//}
//
//// Evaluate
//
//Object_ptr Evaluator::evaluate(Statement_ptr statement)
//{
//	return std::visit(overloaded{
//		[&](Assignment stat) { return evaluate(stat); },
//
//		[&](Branching stat) { return evaluate(stat); },
//
//		[&](WhileLoop stat) { return evaluate(stat); },
//		[&](ForInLoop stat) { return evaluate(stat); },
//
//		[&](Break stat) { return evaluate(stat); },
//		[&](Continue stat) { return evaluate(stat); },
//		[&](Pass stat) { return evaluate(stat); },
//		[&](Return stat) { return evaluate(stat); },
//		[&](YieldStatement stat) { return evaluate(stat); },
//
//		[&](VariableDefinition stat) { return evaluate(stat); },
//		[&](UDTDefinition stat) { return evaluate(stat); },
//		[&](AliasDefinition stat) { return evaluate(stat); },
//		[&](FunctionDefinition stat) { return evaluate(stat); },
//		[&](GeneratorDefinition stat) { return evaluate(stat); },
//		[&](EnumDefinition stat) { return evaluate(stat); },
//
//		[&](ExpressionStatement stat) { return evaluate(stat); },
//		[&](AssertStatement stat) { return evaluate(stat); },
//
//		[&](ImportCustom stat) { return evaluate(stat); },
//		[&](ImportInBuilt stat) { return evaluate(stat); },
//
//		[](auto) { THROW("Never Seen this Statement before!"); }
//		}, *statement);
//}
//
//Object_ptr Evaluator::evaluate(Expression_ptr expression)
//{
//	return std::visit(overloaded{
//			[&](string exp) { return evaluate(exp); },
//			[&](double exp) { return evaluate(exp); },
//			[&](bool exp) { return evaluate(exp); },
//
//			[&](ListLiteral exp) { return evaluate(exp); },
//			[&](TupleLiteral exp) { return evaluate(exp); },
//			[&](MapLiteral exp) { return evaluate(exp); },
//			[&](UDTLiteral exp) { return evaluate(exp); },
//
//			[&](EnumMember exp) { return evaluate(exp); },
//			[&](UDTMemberAccess exp) { return evaluate(exp); },
//
//			[&](Unary exp) { return evaluate(exp); },
//			[&](Binary exp) { return evaluate(exp); },
//
//			[&](Identifier exp) { return evaluate(exp); },
//			[&](Call exp) { return evaluate(exp); },
//
//			[](auto) { THROW("Never Seen this Expression before!"); }
//		}, *expression);
//}
//
//// Statement Evaluation
//
//Object_ptr Evaluator::evaluate(Assignment statement)
//{
//	ASSERT(
//		statement.lhs_expressions.size() == statement.rhs_expressions.size(),
//		"Mismatch in number of identifiers and RHS expressions"
//	);
//
//	//for (int index = 0; index < statement.lhs_expressions.size(); index++)
//	//{
//	//	auto lhs_expression = statement.lhs_expressions[index];
//	//	auto rhs_expression = statement.rhs_expressions[index];
//
//	//	std::visit(overloaded{
//	//		// Assign to Identifier
//
//	//		[&](Identifier left, string) {},
//	//		[&](Identifier left, double) {},
//	//		[&](Identifier left, bool) {},
//
//	//		[&](Identifier left, Identifier right) {},
//	//		[&](Identifier left, ListLiteral right) {},
//	//		[&](Identifier left, TupleLiteral right) {},
//	//		[&](Identifier left, MapLiteral right) {},
//	//		[&](Identifier left, UDTLiteral right) {},
//	//		[&](Identifier left, EnumMember right) {},
//	//		[&](Identifier left, UDTMemberAccess right) {},
//	//		[&](Identifier left, Unary right) {},
//	//		[&](Identifier left, Binary right) {},
//	//		[&](Identifier left, Call right) {},
//
//	//		// Assign to Member Identifier
//
//	//		[&](UDTMemberAccess left, Identifier right) {},
//	//		[&](UDTMemberAccess left, UDTMemberAccess right) {},
//	//		[&](UDTMemberAccess left, Call right) {},
//
//	//		[](auto, auto) { /* THROW("Something is wrong with this assignment!");*/ }
//	//		}, *lhs_expression, *rhs_expression);
//	//}
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(Branching branch)
//{
//	for (auto const [condition_expression, block] : branch.branches)
//	{
//		auto condition = evaluate(condition_expression);
//		THROW_ASSERT(holds_alternative<BooleanObject>(*condition), "Condition has to return bool");
//		auto is_satisfied = get<BooleanObject>(*condition);
//
//		if (!is_satisfied.value)
//			continue;
//
//		env->enter_branch_scope();
//		auto block_result = evaluate_block(move(block));
//		env->leave_scope();
//
//		return move(block_result);
//	}
//
//	env->enter_branch_scope();
//	auto block_result = evaluate_block(move(branch.else_block));
//	env->leave_scope();
//
//	return move(block_result);
//}
//
//Object_ptr Evaluator::evaluate(WhileLoop loop)
//{
//	env->enter_loop_scope();
//
//	while (true)
//	{
//		auto condition = evaluate(loop.condition);
//		THROW_ASSERT(holds_alternative<BooleanObject>(*condition), "Condition has to return bool");
//		auto is_satisfied = get<BooleanObject>(*condition);
//
//		if (!is_satisfied.value)
//			break;
//
//		auto result = evaluate_block(loop.block);
//
//		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
//		{
//			env->leave_scope();
//			return result;
//		}
//		else if (holds_alternative<ContinueObject>(*result))
//		{
//			continue;
//		}
//		else if (holds_alternative<BreakObject>(*result))
//		{
//			break;
//		}
//	}
//
//	env->leave_scope();
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(ForInLoop statement)
//{
//	auto iterable = evaluate(statement.iterable);
//
//	if (holds_alternative<StringObject>(*iterable))
//	{
//		auto variable_name = get<StringObject>(*iterable);
//		auto variable_info = env->get_variable_info(variable_name.value);
//		iterable = variable_info->value;
//	}
//
//	env->enter_loop_scope();
//
//	env->create_variable(
//		statement.item_name,
//		false,
//		true,
//		statement.item_type,
//		MAKE_OBJECT_VARIANT(NoneObject())
//	);
//
//	auto result = VOID;
//	/*std::visit(overloaded{
//		[&](ListObject value)
//		{ return loop_over_iterable(statement.item_name, statement.block, value); },
//		[&](MapObject value)
//		{ return loop_over_iterable(statement.item_name, statement.block, value); },
//
//		[](auto) { THROW("Unable to loop over this object"); }
//		}, *iterable);*/
//
//	env->leave_scope();
//	return result;
//}
//
//Object_ptr Evaluator::evaluate(Break)
//{
//	if (env->is_inside_loop_scope())
//		return MAKE_OBJECT_VARIANT(BreakObject());
//
//	THROW("Break must be used within a loop");
//}
//
//Object_ptr Evaluator::evaluate(Continue)
//{
//	if (env->is_inside_loop_scope())
//		return MAKE_OBJECT_VARIANT(ContinueObject());
//
//	THROW("Continue must be used within a loop");
//}
//
//Object_ptr Evaluator::evaluate(Pass)
//{
//	if (env->is_inside_loop_scope())
//		return MAKE_OBJECT_VARIANT(ContinueObject());
//
//	THROW("Continue must be used within a loop");
//}
//
//Object_ptr Evaluator::evaluate(Return statement)
//{
//	if (env->is_inside_function_scope())
//	{
//		if (statement.expression.has_value())
//		{
//			auto result = evaluate(statement.expression.value());
//			return MAKE_OBJECT_VARIANT(ReturnObject(result));
//		}
//
//		return VOID;
//	}
//
//	THROW("Return must be used inside a function");
//}
//
//Object_ptr Evaluator::evaluate(YieldStatement statement)
//{
//	if (env->is_inside_function_scope())
//	{
//		if (statement.expression.has_value())
//		{
//			auto result = evaluate(statement.expression.value());
//			return MAKE_OBJECT_VARIANT(ReturnObject(result));
//		}
//
//		return VOID;
//	}
//
//	THROW("Return must be used inside a function");
//}
//
//Object_ptr Evaluator::evaluate(VariableDefinition declaration)
//{
//	auto result = evaluate(declaration.expression);
//
//	env->create_variable(
//		declaration.name,
//		declaration.is_public,
//		declaration.is_mutable,
//		declaration.type,
//		move(result)
//	);
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(UDTDefinition def)
//{
//	env->create_UDT(def.name, def.is_public, def.member_types);
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(AliasDefinition def)
//{
//	env->create_alias(def.name, def.is_public, def.type);
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(FunctionDefinition def)
//{
//	env->create_function(
//		def.name,
//		def.is_public,
//		def.arguments,
//		def.return_type,
//		def.body
//	);
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(GeneratorDefinition def)
//{
//	env->create_function(
//		def.name,
//		def.is_public,
//		def.arguments,
//		def.return_type,
//		def.body
//	);
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(EnumDefinition def)
//{
//	std::set<std::string> members_set(def.members.begin(), def.members.end());
//
//	ASSERT(
//		members_set.size() == def.members.size(),
//		"Duplicate enum members are not allowed"
//	);
//
//	env->create_enum(def.name, def.is_public, members_set);
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(ExpressionStatement statement)
//{
//	return evaluate(statement.expression);
//}
//
//Object_ptr Evaluator::evaluate(AssertStatement statement)
//{
//	return evaluate(statement.expression);
//}
//
//Object_ptr Evaluator::evaluate(ImportCustom statement)
//{
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(ImportInBuilt statement)
//{
//	std::string module_name = statement.module_name;
//
//	for (auto const name : statement.goods)
//	{
//		auto function_visitor = get_inbuilt_function_visitor(module_name, name);
//		env->import_builtin(name, function_visitor);
//	}
//
//	return VOID;
//}
//
//// Expression Evaluation
//
//Object_ptr Evaluator::evaluate(string string_literal)
//{
//	return MAKE_OBJECT_VARIANT(StringObject(string_literal));
//}
//
//Object_ptr Evaluator::evaluate(double number_literal)
//{
//	return MAKE_OBJECT_VARIANT(NumberObject(number_literal));
//}
//
//Object_ptr Evaluator::evaluate(bool bool_literal)
//{
//	return MAKE_OBJECT_VARIANT(BooleanObject(bool_literal));
//}
//
//Object_ptr Evaluator::evaluate(ListLiteral expression)
//{
//	auto list_object = ListObject();
//
//	for (const auto expression : expression.expressions)
//	{
//		auto result = evaluate(expression);
//		list_object.append(result);
//	}
//
//	return MAKE_OBJECT_VARIANT(list_object);
//}
//
//Object_ptr Evaluator::evaluate(TupleLiteral expression)
//{
//	std::vector<Object_ptr> values;
//
//	for (const auto expression : expression.expressions)
//	{
//		auto result = evaluate(expression);
//		values.push_back(move(result));
//	}
//
//	auto tuple_object = TupleObject(values);
//
//	return MAKE_OBJECT_VARIANT(tuple_object);
//}
//
//Object_ptr Evaluator::evaluate(MapLiteral expression)
//{
//	auto map_object = MapObject();
//
//	for (auto const& [key, value_expr] : expression.pairs)
//	{
//		auto value_object = evaluate(value_expr);
//		//map_object.insert(MAKE_OBJECT_VARIANT(key->value), value_object);
//	}
//
//	return MAKE_OBJECT_VARIANT(map_object);
//}
//
//Object_ptr Evaluator::evaluate(UDTLiteral expression)
//{
//	auto UDT_object = UDTObject();
//
//	for (auto const& [key, value_expr] : expression.pairs)
//	{
//		auto value_object = evaluate(value_expr);
//		UDT_object.set(MAKE_OBJECT_VARIANT(key->value), value_object);
//	}
//
//	return MAKE_OBJECT_VARIANT(UDT_object);
//}
//
//Object_ptr Evaluator::evaluate(UDTMemberAccess expression)
//{
//	//auto container = evaluate(expression.container);
//	//auto access_expression = evaluate(expression.access_expression);
//
//	//auto name = get<Identifier>(*container);
//	//auto info = env->get_variable_info(name);
//
//	//return visit(overloaded{
//	//	//[&](UDTObject& container, Identifier& identifier) {},
//
//	//	[](auto, auto) { THROW("Unable to perform member access"); }
//	//	}, *container, *access_expression);
//
//	THROW("Unable to perform member access");
//}
//
//Object_ptr Evaluator::evaluate(EnumMember expression)
//{
//	auto enum_info = env->get_enum_info(expression.enum_name);
//
//	if (enum_info->members.contains(expression.member_name))
//	{
//		return MAKE_OBJECT_VARIANT(
//			EnumMemberObject(expression.enum_name, expression.member_name)
//		);
//	}
//
//	THROW("Enum " + expression.enum_name + " does not contain " + expression.member_name);
//}
//
//Object_ptr Evaluator::evaluate(Identifier expression)
//{
//	auto info = env->get_variable_info(expression.name);
//	return move(info->value);
//}
//
//Object_ptr Evaluator::evaluate(Call call_expression)
//{
//	/*if (auto info = env->get_inbuilt_function_info_if_exists(call_expression.name))
//	{
//		return evaluate_function_call(call_expression, info);
//	}
//
//	vector<Object_ptr> formal_arguments;
//
//	for (auto const& argument : call_expression.arguments)
//	{
//		auto object = evaluate(argument);
//		formal_arguments.push_back(object);
//	}
//
//	auto info = env->get_info(call_expression.name);
//	auto function_info = get_if<FunctionInfo>(&*info);
//
//	return evaluate_function_call(call_expression, function_info, formal_arguments);*/
//
//	return VOID;
//}
//
//Object_ptr Evaluator::evaluate(Unary unary_expression)
//{
//	auto operand = evaluate(unary_expression.operand);
//	auto token_type = unary_expression.op->type;
//
//	return std::visit(overloaded{
//		[&](NumberObject number) { return perform_operation(token_type, number.value); },
//		[&](BooleanObject boolean) { return perform_operation(token_type, boolean.value); },
//
//		[](auto) { THROW("The Unary Operation is not defined for this operand"); }
//		}, *operand);
//}
//
//Object_ptr Evaluator::evaluate(Binary binary_expression)
//{
//	auto left_variant = evaluate(binary_expression.left);
//	auto right_variant = evaluate(binary_expression.right);
//	auto token_type = binary_expression.op->type;
//
//	return std::visit(overloaded{
//		[&](BooleanObject left, BooleanObject right) { return perform_operation(token_type, left.value, right.value); },
//		[&](NumberObject left, NumberObject right) { return perform_operation(token_type, left.value, right.value); },
//		[&](StringObject left, StringObject right) { return perform_operation(token_type, left.value, right.value); },
//		[&](StringObject left, NumberObject right) { return perform_operation(token_type, left.value, right.value); },
//
//		[](auto, auto) { THROW("The Binary Operation is not defined for these operands"); }
//		}, *left_variant, *right_variant);
//}
//
//// Perform Operation
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, double operand)
//{
//	switch (token_type)
//	{
//	case WTokenType::UNARY_MINUS:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(-operand));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, bool operand)
//{
//	switch (token_type)
//	{
//	case WTokenType::BANG:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(!operand));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, bool left, bool right)
//{
//	switch (token_type)
//	{
//	case WTokenType::EQUAL_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
//	}
//	case WTokenType::BANG_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
//	}
//	case WTokenType::AND:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left && right));
//	}
//	case WTokenType::OR:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left || right));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, double left, double right)
//{
//	switch (token_type)
//	{
//	case WTokenType::POWER:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(std::pow(left, right)));
//	}
//	case WTokenType::DIVISION:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(left / right));
//	}
//	case WTokenType::REMINDER:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(std::remainder(left, right)));
//	}
//	case WTokenType::STAR:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(left * right));
//	}
//	case WTokenType::PLUS:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(left + right));
//	}
//	case WTokenType::MINUS:
//	{
//		return MAKE_OBJECT_VARIANT(NumberObject(left - right));
//	}
//	case WTokenType::GREATER_THAN:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left > right));
//	}
//	case WTokenType::GREATER_THAN_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left >= right));
//	}
//	case WTokenType::LESSER_THAN:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left < right));
//	}
//	case WTokenType::LESSER_THAN_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left <= right));
//	}
//	case WTokenType::EQUAL_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
//	}
//	case WTokenType::BANG_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, string& left, string& right)
//{
//	switch (token_type)
//	{
//	case WTokenType::PLUS:
//	{
//		return MAKE_OBJECT_VARIANT(StringObject(left + right));
//	}
//	case WTokenType::EQUAL_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left == right));
//	}
//	case WTokenType::BANG_EQUAL:
//	{
//		return MAKE_OBJECT_VARIANT(BooleanObject(left != right));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//Object_ptr Evaluator::perform_operation(WTokenType token_type, string& left, double right)
//{
//	switch (token_type)
//	{
//	case WTokenType::STAR:
//	{
//		int count = 0;
//		auto repetitions = right;
//
//		string result = "";
//
//		while (count < repetitions)
//		{
//			result += left;
//			count++;
//		}
//
//		return MAKE_OBJECT_VARIANT(StringObject(result));
//	}
//	}
//
//	THROW("Operation not supported");
//}
//
//// Loop over iterable
//
//Object_ptr Evaluator::loop_over_iterable(string item_name, Block block, ListObject& list_object)
//{
//	for (auto const& element : list_object.values)
//	{
//		env->set_variable(item_name, element);
//		auto result = evaluate_block(block);
//
//		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
//		{
//			env->leave_scope();
//			return result;
//		}
//		else if (holds_alternative<ContinueObject>(*result))
//		{
//			continue;
//		}
//		else if (holds_alternative<BreakObject>(*result))
//		{
//			break;
//		}
//	}
//
//	return VOID;
//}
//
//Object_ptr Evaluator::loop_over_iterable(string item_name, Block block, MapObject& dict_object)
//{
//	/*for (auto const& [key, value] : dict_object.pairs)
//	{
//		NULL_CHECK(value);
//
//		Object_ptr key_object = std::visit(overloaded{
//			[](NumberObject num) { return MAKE_OBJECT_VARIANT(num); },
//			[](StringObject text) { return MAKE_OBJECT_VARIANT(text); },
//			[](BooleanObject boolean) { return MAKE_OBJECT_VARIANT(boolean); },
//
//			[&](auto) { THROW("Cannot iterate over this datatype"); }
//			}, *key);
//
//		env->set_variable(
//			item_name,
//			MAKE_OBJECT_VARIANT(TupleObject({ key_object, value }))
//		);
//
//		auto result = evaluate_block(block);
//
//		if (holds_alternative<ReturnObject>(*result) || holds_alternative<ErrorObject>(*result))
//		{
//			env->leave_scope();
//			return result;
//		}
//		else if (holds_alternative<ContinueObject>(*result))
//		{
//			continue;
//		}
//		else if (holds_alternative<BreakObject>(*result))
//		{
//			break;
//		}
//	}*/
//
//	return VOID;
//}
//
//// Evaluate function
//
//Object_ptr Evaluator::evaluate_function_call(Call call_expression, FunctionInfo* info, vector<Object_ptr> formal_arguments)
//{
//	int index = 0;
//
//	for (auto const& argument : info->arguments)
//	{
//		auto argument_name = argument.first;
//		auto argument_type = argument.second;
//
//		auto object = formal_arguments[index];
//
//		env->create_variable(
//			argument_name,
//			false,
//			true,
//			argument_type,
//			move(object)
//		);
//
//		index++;
//	}
//
//	auto result = evaluate_block(info->body);
//
//	if (holds_alternative<ReturnObject>(*result))
//	{
//		return result;
//	}
//
//	FATAL("Function must return a ReturnObject");
//}
//
//Object_ptr Evaluator::evaluate_function_call(Call call_expression, InBuiltFunctionInfo* info)
//{
//	vector<Object_ptr> argument_objects;
//
//	for (auto const& argument : call_expression.arguments)
//	{
//		auto result = evaluate(argument);
//		argument_objects.push_back(move(result));
//	}
//
//	return info->func(argument_objects);
//}
//
//// Utils
//
//Object_ptr Evaluator::evaluate_block(Block block)
//{
//	for (auto& statement : block)
//	{
//		auto result = evaluate(statement);
//
//		if (
//			holds_alternative<ReturnObject>(*result) ||
//			holds_alternative<ErrorObject>(*result) ||
//			holds_alternative<ContinueObject>(*result) ||
//			holds_alternative<BreakObject>(*result)
//			) {
//			return result;
//		}
//	}
//
//	return VOID;
//}