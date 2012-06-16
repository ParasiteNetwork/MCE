/*

	mce-vars.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#include "mce-vars.h"
#include "basic_latin.h"

#include <string>

namespace mce {
	namespace vars {
		unsigned char const mce_tab_char = basic_latin::control::tab;
		unsigned char const mce_space_char = basic_latin::symbol::space;
		unsigned char const mce_comma_char = basic_latin::symbol::comma;
		unsigned char const mce_equal_char = basic_latin::symbol::equals_sign;
		unsigned char const mce_not_equal_char = basic_latin::symbol::number_sign;
		unsigned char const mce_text_mark_char = basic_latin::symbol::quotation_mark;
		std::string const mce_define = "define";
		std::string const mce_undefine = "undefine";
		std::string const mce_if = "if";
		std::string const mce_ifnot = "ifnot";
		std::string const mce_elseif = "elsif";
		std::string const mce_elseifnot = "elsifnot";
		std::string const mce_else = "else";
		std::string const mce_endif = "endif";
		std::string const mce_include = "include";
		std::string const mce_false = "FALSE";
		std::string const mce_true = "TRUE";
		unsigned char const mce_escape_char = basic_latin::symbol::dollar_sign;
		unsigned char const mce_scope_open_char = basic_latin ::symbol::left_paranthesis;
		unsigned char const mce_scope_close_char = basic_latin::symbol::right_paranthesis;
	}
}
