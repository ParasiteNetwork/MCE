/*

	mce-syntax.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#ifndef mce_syntaxH_bfd4e1a1_fbaa_467f_9fb6_0167e6ae9515
#define mce_syntaxH_bfd4e1a1_fbaa_467f_9fb6_0167e6ae9515

#include <string>

#include "forwarding.h"


namespace mce {

	/*
		Forwarding.
	*/
	FORWARD(env)
	FORWARD(scope)
	FROM_NAMESPACE_FORWARD(apt, form)

	/*
		Only if the textual boolean explicitly is "FALSE" or empty, is it considered
		false. All other representations are true.
	*/
	bool translate_textual_boolean(std::string const & boolean);

	namespace syntax
	{
		bool if_template(env & e, apt::form const * form, bool condition, std::string const & id);
		bool if_(env & e, apt::form const * form);
		bool ifnot(env & e, apt::form const * form);

		bool elsif_template(env & e, apt::form const * form, bool condition, std::string const & id);
		bool elsif(env & e, apt::form const * form);
		bool elsifnot(env & e, apt::form const * form);
		
		bool else_(env & e, apt::form const * form);
		bool endif(env & e, apt::form const * form);
		bool define(env & e, apt::form const * form);
		bool undefine(env & e, apt::form const * form);
		bool include(env & e, apt::form const * form);
	} // namespace syntax
} // namespace mce

#endif