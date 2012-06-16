/*

	mce-scope.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/

#include "mce-scope.h"

namespace mce {

//------------------------------------------------------------------------------
void scope::push_scope(state s) 
{
	d_scopes.push_back(s);
}
//------------------------------------------------------------------------------
void scope::change_scope(state s) 
{
	pop_scope();
	push_scope(s);
}
//------------------------------------------------------------------------------
bool scope::pop_scope()
{
	if(d_scopes.empty()) {
		return false;
	}
	d_scopes.pop_back();
	return true;
}
//------------------------------------------------------------------------------
bool scope::check_scope(state s) const
{
	if(!d_scopes.empty()) {
		return d_scopes.back() == s;
	}
	return s == open;
}
//------------------------------------------------------------------------------
bool scope::top_level() const
{
	return d_scopes.empty();
}
//------------------------------------------------------------------------------
void scope::reset()
{
	d_scopes.clear();
}
//------------------------------------------------------------------------------

} // namespace mce
