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
