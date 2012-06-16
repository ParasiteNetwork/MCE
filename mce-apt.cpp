/*

	mce-apt.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#include <sstream>
#include <vector>
#include <string>

#include "mce-apt.h"
#include "mce-env.h"
#include "mce-vars.h"
#include "debug.h"


namespace mce {
	namespace apt {

//------------------------------------------------------------------------------
/* BASE */
//------------------------------------------------------------------------------
base::~base()
{

}
//------------------------------------------------------------------------------
/* FORM */
//------------------------------------------------------------------------------
form::form() : d_expecting_macro_id(true)
{

}
//------------------------------------------------------------------------------
form::form(text * macro_id) : d_expecting_macro_id(true)
{
	/*
		Perhaps we should throw an exception here?
	*/
	(void) add_argument(macro_id);
}
//------------------------------------------------------------------------------
bool form::add_argument(base * argument)
{
	if(!argument) {
		debugf("MCE(apt::form::add_argument): Null pointer!");
		return false;
	}
	/*
		The first argument is always the actual macro id. It must be an apt::text
		object or it fails.
	*/
	if(d_expecting_macro_id) {
		text * macro_id = dynamic_cast<text *>(argument);
		if(!macro_id) {
			debugf("MCE(apt::form::add_argument): First argument to the form is invalid!");
			debugf("MCE(apt::form::add_argument): Must be a static text identifier.");
			return false;
		}
		d_form_macro_id = macro_id->value();
		delete macro_id;
		d_expecting_macro_id = false;
	} else {
		d_arguments.push_back(argument);
	}
	return true;
}
//------------------------------------------------------------------------------
bool form::eval(env & e, std::ostringstream & out, bool root) const
{
	/*
		If the form is a syntax macro we'll let it handle the evaluation.
		If it's a user macro we'll evaluate all arguments and pass them to the
		user supplied function for the macro.

		Only on "root" forms do we allow syntax macros. All sub forms are not allowed
		to contain them. All recursive calls sets the root boolean to false. It's
		only true on the first call from the parser.

		User macros are only evaluated if the scope is open.
	*/
	if(e.is_syntax_macro(d_form_macro_id)) {
		if(!root) {
			debugf("MCE(apt::form::eval): Syntax macros can't be used in sub forms!");
			return false;
		}
		SyntaxMacro callback;
		if(!e.get_syntax_macro(d_form_macro_id,callback)) {
			debugf("MCE(apt::form::eval): Syntax macro couldn't be found: %s", d_form_macro_id.c_str());
			return false;
		}
		return callback(e,this);
	} else 
	if(e.is_user_macro(d_form_macro_id)) {
		if(!e.get_scope().check_scope(scope::open)) {
			return false;
		}
		UserMacro callback;
		if(!e.get_user_macro(d_form_macro_id, callback)) {
			debugf("MCE(apt::form::eval): User macro couldn't be found: %s", d_form_macro_id.c_str());
			return false;
		}
		std::vector<std::string> evaluated_arguments;
		std::ostringstream local_out;
		for(auto argument: d_arguments) {
			local_out.clear();
			if(!argument->eval(e,local_out,false)) {
				debugf("MCE(apt::form::eval): Evaluation of sub form failed!");
				return false;
			}
			evaluated_arguments.push_back(local_out.str());
		}
		return callback(evaluated_arguments, out);
	} else {
		debugf("MCE(apt::form::eval): Fatality! Unkown macro id in form: %s!", d_form_macro_id.c_str());
		return false;
	}
}
//------------------------------------------------------------------------------
form::const_iterator form::begin() const
{
	return d_arguments.begin();
}
//------------------------------------------------------------------------------
form::const_iterator form::end() const
{
	return d_arguments.end();
}
//------------------------------------------------------------------------------
bool form::is_callable() const
{
	return !d_expecting_macro_id;
}
//------------------------------------------------------------------------------
bool form::has_arguments() const
{
	return !d_arguments.empty();
}
//------------------------------------------------------------------------------
form::~form()
{
	for(unsigned i = 0; i < d_arguments.size(); ++i) {
		delete d_arguments[i];
	}
}
//------------------------------------------------------------------------------
std::string const & form::get_macro_id() const
{
	return d_form_macro_id;
}
//------------------------------------------------------------------------------
/* TEXT */
//------------------------------------------------------------------------------
text::text(std::string const & text) : d_text(text)
{

}
//------------------------------------------------------------------------------
bool text::define(env & e) const
{
	return e.define(d_text);
}
//------------------------------------------------------------------------------
bool text::undefine(env & e) const
{
	return e.undefine(d_text);
}
//------------------------------------------------------------------------------
std::string const & text::value() const
{
	return d_text;
}
//------------------------------------------------------------------------------
bool text::eval(env & e, std::ostringstream & out, bool root) const
{
	out << d_text;
	return true;
}
//------------------------------------------------------------------------------
/* BINOP */
//------------------------------------------------------------------------------
binop::binop(base * first, base * second) : d_first(first), d_second(second)
{
	if(!first || !second) {
		debugf("MCE(apt::binop): Null pointer violation! Expect crash!");
	}
}
//------------------------------------------------------------------------------
binop::~binop()
{
	delete d_first;
	delete d_second;
}
//------------------------------------------------------------------------------
bool binop::check_equality(env & e) const
{
	std::ostringstream first_out;
	std::ostringstream second_out;
	if(!d_first->eval(e,first_out,false)) {
		debugf("MCE(apt::binop::check_equality): LHS failed evaluation!");
		return false;
	}
	if(!d_second->eval(e,second_out,false)) {
		debugf("MCE(apt::binop::check_equality): RHS failed evaluation!");
		return false;
	}
	if(first_out.str() == second_out.str()) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
/* NOT_EQUAL */
//------------------------------------------------------------------------------
not_equal::not_equal(base * first, base * second) : binop(first,second)
{
	
}
//------------------------------------------------------------------------------
bool not_equal::eval(env & e, std::ostringstream & out, bool root) const
{
	if(check_equality(e)) {
		out << vars::mce_false;
	} else {
		out << vars::mce_true;
	}
	return true;
}
//------------------------------------------------------------------------------
/* EQUAL */
//------------------------------------------------------------------------------
equal::equal(base * first, base * second) : binop(first,second)
{

}
//------------------------------------------------------------------------------
bool equal::define(env & e) const
{
	text * id = dynamic_cast<text *>(d_first);
	if(!id) {
		debugf("MCE(apt::equal::define): LHS must be a text identifier in a defintion!");
		return false;
	}
	std::ostringstream out;
	if(!d_second->eval(e,out,false)) {
		debugf("MCE(apt::equal::define): RHS failed evaluation!");
		return false;
	}
	if(!e.define(id->value(),out.str())) {
		debugf("MCE(apt::equal::define): Failed to make the definition!");
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------
bool equal::eval(env & e, std::ostringstream & out, bool root) const
{
	if(check_equality(e)) {
		out << vars::mce_true;
	} else {
		out << vars::mce_false;
	}
	return true;
}
//------------------------------------------------------------------------------

	} // namespace apt
} // namespace mce
