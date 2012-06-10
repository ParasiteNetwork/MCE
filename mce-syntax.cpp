/*
Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <string>
#include <sstream>
#include <vector>

#include "mce-syntax.h"
#include "mce-scope.h"
#include "mce-env.h"
#include "mce-vars.h"
#include "mce-apt.h"
#include "debug.h"


namespace mce {

//------------------------------------------------------------------------------
bool translate_textual_boolean(std::string const & boolean)
{
	if(boolean.empty() || boolean == vars::mce_false) {
		return false;
	}
	return true;
}
//------------------------------------------------------------------------------

} // namespace mce

namespace mce {
	namespace syntax {

//------------------------------------------------------------------------------
bool if_template(env & e, apt::form const * form, bool condition, std::string const & id)
{
	/*
		Sanity checks. Make sure we have a proper form.
	*/
	if(!form) {
		debugf("MCE(syntax::if_template): %s called with NULL form!", id.c_str());
		return false;
	}
	if(!form->has_arguments()) {
		debugf("MCE(syntax::if_template): %s called with an empty form!", id.c_str());
		return false;
	}
	scope & s = e.get_scope();
	/*
		If the current scope is either closed or deferred we simply close the
		scope for this nested if statement. Blocking any evaluation.
	*/
	if(!s.check_scope(scope::open)) {
		s.push_scope(scope::closed);
		return true;
	}
	/*
		We loop through each condition expression and evaluate it. If any of
		them evaluates to NOT(condition), then we defer the condition.
	*/
	for(auto argument: *form) {
		std::ostringstream out;
		if(!argument->eval(e,out,false)) {
			debugf("MCE(syntax::if_template): Evaluation of sub form failed!");
			return false;
		}
		if(!translate_textual_boolean(out.str()) == condition) {
			s.push_scope(scope::deferred);
			return true;
		}
	}
	/*
		All conditions are true. Open the nested scope.
	*/
	s.push_scope(scope::open);
	return true;
}
//------------------------------------------------------------------------------
bool if_(env & e, apt::form const * form)
{
	return if_template(e,form,true,"IF");
}
//------------------------------------------------------------------------------
bool ifnot(env & e, apt::form const * form)
{
	return if_template(e,form,false,"IFNOT");
}
//------------------------------------------------------------------------------
bool elsif_template(env & e, apt::form const * form, bool condition, std::string const & id)
{
	/*
		Sanity checks. Make sure we have a proper form.
	*/
	if(!form) {
		debugf("MCE(syntax::elsif_template): %s called with NULL form!", id.c_str());
		return false;
	}
	if(!form->has_arguments()) {
		debugf("MCE(syntax::elsif_template): %s called with an empty form!", id.c_str());
		return false;
	}
	scope & s = e.get_scope();
	/*
		ELSE, ELSIF and ELSEIFNOT are not allowed in the top level scope.
	*/
	if(s.top_level()) {
		debugf("MCE(syntax::elsif_template): %s is not allowed without an IF!", id.c_str());
		return false;
	}
	/*
		If the current scope is already open it means that a previous if
		has already evaluated to true and it's body has been parsed. Thus
		we close all subsequent if clauses. It's like ENDIF, but we do not pop.
	*/
	if(s.check_scope(scope::open)) {
		s.change_scope(scope::closed);
		return true;
	}
	/*
		If the current scope is deferred, then we will evaluate the condition
		expressions. If any of them do not match 'condition', we return without
		changing the scope status.
	*/
	if(s.check_scope(scope::deferred)) {
		for(auto argument: *form) {
			std::ostringstream out;
			if(!argument->eval(e,out,false)) {
				debugf("MCE(syntax::elsif_template): Evaluation of sub form failed!");
				return false;
			}
			if(translate_textual_boolean(out.str()) == condition) {
				/*
					Perhaps it feels more natural to return false here since
					the test to open the scope failed, but since we already
					are in a deferred state, this is merely a test to change
					the scope status. If we would return false, we are actually
					saying that some syntax or semantic error occurred and the
					entire statement is malformed and should thus be ignored.
					This is not the case and true here signals that the function
					has executed without any such encountered errors, even if
					the condition failed to open the scope.
				*/
				return true;
			}
		}
		s.change_scope(scope::open);
	}
	/*
		If the current scope is closed, we do nothing.
	*/
	return true;	
}
//------------------------------------------------------------------------------
bool elsif(env & e, apt::form const * form)
{
	return elsif_template(e,form,true,"ELSIF");
}
//------------------------------------------------------------------------------
bool elsifnot(env & e, apt::form const * form)
{
	return elsif_template(e,form,false,"ELSIFNOT");
}
//------------------------------------------------------------------------------
bool else_(env & e, apt::form const * form)
{
	/*
		Sanity checks.
	*/
	if(!form) {
		debugf("MCE(syntax::else_): ELSE called with NULL form!");
		return false;
	}
	if(form->has_arguments()) {
		debugf("MCE(syntax::else_): ELSE called with arguments!");
		return false;
	}
	scope & s = e.get_scope();
	/*
		ELSE, ELSIF and ELSEIFNOT are not allowed in the top level scope.
	*/
	if(s.top_level()) {
		debugf("MCE(syntax::else_): ELSE is not allowed without an IF!");
		return false;
	}
	/*
		If the current scope is already open it means that a previous if
		has already evaluated to true and it's body has been parsed. Thus
		we close all subsequent if clauses. It's like ENDIF, but we do not pop.
	*/
	if(s.check_scope(scope::open)) {
		s.change_scope(scope::closed);
	}
	/*
		If the current scope is deferred, then we'll open it.
	*/
	else if(s.check_scope(scope::deferred)) {
		s.change_scope(scope::open);
	}
	/*
		If the current scope is closed, we do nothing.
	*/
	return true;	
}
//------------------------------------------------------------------------------
bool endif(env & e, apt::form const * form)
{
	/*
		Sanity checks.
	*/
	if(!form) {
		debugf("MCE(syntax::endif): ENDIF called with NULL form!");
		return false;
	}
	if(form->has_arguments()) {
		debugf("MCE(syntax::endif): ENDIF called with arguments!");
		return false;
	}
	scope & s = e.get_scope();
	/*
		An ENDIF is not allowed at the top level.
	*/
	if(s.top_level()) {
		return false;
	}
	/*
		We pop the scope stack.
	*/
	return s.pop_scope();
}
//------------------------------------------------------------------------------
bool define(env & e, apt::form const * form)
{
	/*
		Sanity checks.
	*/
	if(!form) {
		debugf("MCE(syntax::define): DEFINE called with NULL form!");
		return false;
	}
	if(!form->has_arguments()) {
		debugf("MCE(syntax::define): DEFINE called with an empty form!");
		return false;
	}
	scope & s = e.get_scope();
	/*
		If the scope isn't open, we won't accept any defines.
	*/
	if(!s.check_scope(scope::open)) {
		return true;
	}
	/*
		(define,name = Anna)

		We check that all arguments are semantically sane. If any fail to
		be defined we simply ignore it.
	*/
	std::vector<apt::definable *> defines;
	for(auto argument: *form) {
		apt::definable * target = dynamic_cast<apt::definable *>(argument);
		if(target) {
			defines.push_back(target);
		} else {
			debugf("MCE(syntax::define): DEFINE called with improper argument!");
			return false;
		}
	}
	for(auto def: defines) {
		if(!def->define(e)) {
			debugf("MCE(syntax::define): Defining failed! Ignoring.");
		}
	}
	return true;
}
//------------------------------------------------------------------------------
bool undefine(env & e, apt::form const * form)
{
	/*
		Sanity checks.
	*/
	if(!form) {
		debugf("MCE(syntax::undefine): UNDEFINE called with NULL form!");
		return false;
	}
	if(!form->has_arguments()) {
		debugf("MCE(syntax::undefine): UNDEFINE called with an empty form!");
		return false;
	}
	scope & s = e.get_scope();
	/*
		If the scope isn't open, we won't accept any undefs.
	*/
	if(!s.check_scope(scope::open)) {
		return true;
	}
	/*
		We only undef explicitly named id's.
	*/
	std::vector<apt::undefinable *> undefines;
	for(auto argument: *form) {
		apt::undefinable * target = dynamic_cast<apt::undefinable *>(argument);
		if(target) {
			undefines.push_back(target);
		} else {
			debugf("MCE(syntax::undefine): UNDEFINE called with improper argument type!");
			return false;
		}
	}
	for(auto undef: undefines) {
		if(!undef->undefine(e)) {
			debugf("MCE(syntax::undefine): Undefining failed! Ignoring.");
		}
	}
	return true;	
}
//------------------------------------------------------------------------------

	} // namespace syntax
} // namespace mce