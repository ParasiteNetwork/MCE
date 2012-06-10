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


#ifndef mce_envH_0817c959_b1cb_4f9f_b35f_b5f57b1f74a4
#define mce_envH_0817c959_b1cb_4f9f_b35f_b5f57b1f74a4

#include <map>
#include <functional>
#include <vector>
#include <string>
#include <sstream>

#include "forwarding.h"
#include "mce-scope.h"


namespace mce {

	/*
		Forwarding.
	*/
	FORWARD(env)
	FORWARD(scope)
	FROM_NAMESPACE_FORWARD(apt, form)

	/*
		This is the signature for the syntax statement functions.
		bool (env & e, apt::form * form)
	*/
	typedef std::function<bool (env &, apt::form const *)> SyntaxMacro;
	/*
		This is the signature for user defined macro callbacks.
		void (std::vector<std::string> const & args, std::ostream & out)
	*/
	typedef std::function<bool (std::vector<std::string> const &, std::ostringstream &)> UserMacro;

	class env {
		private:
			std::map<std::string, UserMacro> d_user_macros;
			std::map<std::string, SyntaxMacro> d_syntax_macros;
			scope d_scope;
		public:
			/*
				Constructor. 
			*/
			env();
			/*
				Defines an macro. It will be bound to an anonymous
				function which will return TRUE on evocation.
				Redefining an already defined macro will fail.
			*/
			bool define(std::string const & id);
			/*
				Defines an macro and bounds it to the user
				supplied function. Trying to redefine an already defined
				macro is an error.
			*/
			bool define(std::string const & id, UserMacro macro);
			/*
				Defines a macro with a static expansion.
			*/
			bool define(std::string const & id, std::string const & expansion);
			/*
				Undefines the macro.
				Trying to undefine a syntax macro will fail.
			*/
			bool undefine(std::string const & id);
			/*
				Undefines all user macros.
			*/
			void undefine_all();
			/*
				Checks if a macro is defined. Works for both
				user macros and syntax macros.
			*/
			bool is_defined(std::string const & id) const;
			/*
				Checks if the macro is a user macro.
			*/
			bool is_user_macro(std::string const & id) const;
			/*
				Checks if the macro is a syntax macro.
			*/
			bool is_syntax_macro(std::string const & id) const;
			/*
				Retrieves the syntax macro.
			*/
			bool get_syntax_macro(std::string const & id, SyntaxMacro & macro);
			/*
				Retrieves the user macro.
			*/
			bool get_user_macro(std::string const & id, UserMacro & macro);
			/*
				Get the scope object.
			*/
			scope & get_scope();
	};

} // namespace mce

#endif
