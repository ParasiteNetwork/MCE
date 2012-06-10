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