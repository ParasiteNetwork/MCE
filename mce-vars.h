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


#ifndef mce_varsH_11d6cfee_7624_4cd6_90eb_9e5271242af6
#define mce_varsH_11d6cfee_7624_4cd6_90eb_9e5271242af6

#include <string>

namespace mce {
	namespace vars {
		// constant values
		extern unsigned char const mce_tab_char;
		extern unsigned char const mce_space_char;
		extern unsigned char const mce_comma_char;
		extern unsigned char const mce_equal_char;
		extern unsigned char const mce_not_equal_char;
		extern unsigned char const mce_text_mark_char;
		extern std::string const mce_macro;
		extern std::string const mce_define;
		extern std::string const mce_undefine;
		extern std::string const mce_if;
		extern std::string const mce_ifnot;
		extern std::string const mce_elseif;
		extern std::string const mce_elseifnot;
		extern std::string const mce_else;
		extern std::string const mce_endif;
		extern std::string const mce_include;
		extern std::string const mce_false;
		extern std::string const mce_true;
		extern unsigned char const mce_escape_char;
		extern unsigned char const mce_scope_open_char;
		extern unsigned char const mce_scope_close_char;
	}
}

#endif
