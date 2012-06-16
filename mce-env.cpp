/*

	mce-env.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/

#include <vector>
#include <string>
#include <sstream>

#include "mce-env.h"
#include "mce-vars.h"
#include "mce-syntax.h"


namespace mce {

//------------------------------------------------------------------------------
env::env()
{
	d_syntax_macros[vars::mce_if] = syntax::if_;
	d_syntax_macros[vars::mce_ifnot] = syntax::ifnot;
	d_syntax_macros[vars::mce_elseif] = syntax::elsif;
	d_syntax_macros[vars::mce_elseifnot] = syntax::elsifnot;
	d_syntax_macros[vars::mce_else] = syntax::else_;
	d_syntax_macros[vars::mce_endif] = syntax::endif;
	d_syntax_macros[vars::mce_define] = syntax::define;
	d_syntax_macros[vars::mce_undefine] = syntax::undefine;
}
//------------------------------------------------------------------------------
bool env::define(std::string const & id)
{
	if(!is_defined(id)) {
		d_user_macros[id] = [](std::vector<std::string> const &, std::ostringstream & out) -> bool {
			/*
				Ignores any arguments. Perhaps it should be an error to
				supply arguments?
			*/
			out << vars::mce_true;
			return true;
		};
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::define(std::string const & id, UserMacro macro)
 {
 	if(!is_defined(id)) {
 		d_user_macros[id] = macro;
 		return true;
 	}
 	return false;
 }
//------------------------------------------------------------------------------
bool env::define(std::string const & id, std::string const & expansion)
{
	if(!is_defined(id)) {
		d_user_macros[id] = [=](std::vector<std::string> const &, std::ostringstream & out) -> bool {
			/*
				Ignores any arguments. Perhaps it should be an error to
				supply arguments?
			*/
			out << expansion;
			return true;
		};
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::undefine(std::string const & id)
{
	if(is_defined(id)) {
		d_user_macros.erase(d_user_macros.find(id));
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
void env::undefine_all()
{
	d_user_macros.clear();
}
//------------------------------------------------------------------------------
bool env::is_defined(std::string const & id) const
{
	if(d_user_macros.count(id) > 0 || d_syntax_macros.count(id) > 0) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::is_syntax_macro(std::string const & id) const
{
	if(d_syntax_macros.count(id) > 0) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::is_user_macro(std::string const & id) const
{
	if(d_user_macros.count(id) > 0) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::get_syntax_macro(std::string const & id, SyntaxMacro & macro)
{
	if(is_syntax_macro(id)) {
		macro = d_syntax_macros[id];
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool env::get_user_macro(std::string const & id, UserMacro & macro)
{
	if(is_user_macro(id)) {
		macro = d_user_macros[id];
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
scope & env::get_scope()
{
	return d_scope;
}
//------------------------------------------------------------------------------

} // namespace mce
