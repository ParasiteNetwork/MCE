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


#ifndef mce_parserH_D314FB22_6C63_11DD_91AF_3C7855D89593
#define mce_parserH_D314FB22_6C63_11DD_91AF_3C7855D89593

#include <iterator>
#include <map>
#include <fstream>
#include <string>

#include "forwarding.h"


namespace mce {

	/*
		Forwarding.
	*/
	FORWARD(env)
	FORWARD(stream_cursor)
	FROM_NAMESPACE_FORWARD(apt, base)
	FROM_NAMESPACE_FORWARD(apt, text)

	/*
		The parser class.
	*/
	class parser {
		private:
			/*
				The user supplied environment.
			*/
			env * d_env;
			/*
				The output file.
				TODO: Should probably use a std::ostream instead.
			*/
			std::ofstream d_out;
			/*
				Intendation counting. Used when macros expand over
				multiple lines.
			*/
			std::string d_intend;
		private:
			bool trim(stream_cursor & in) const;
			void print(unsigned char const & c);
			void print(std::string const & text);
			bool parse_stream(stream_cursor & in);
			bool check_macro_escape_character(stream_cursor & in) const;
			bool parse_macro_escape_character(stream_cursor & in) const;
			bool parse_macro(stream_cursor & in, apt::base *& node) const;
			bool parse_macro_proper_form(stream_cursor & in, apt::base *& node) const;
			bool parse_macro_argument(stream_cursor & in, apt::base *& node) const;
			bool parse_macro_syntactic_suger_form(stream_cursor & in, apt::base *& node) const;
			bool parse_binop(stream_cursor & in, apt::base *& node) const;
			bool validate_callback(std::string const & id) const;
			bool trim_commas(stream_cursor & in) const;
			bool parse_quoted_text(stream_cursor & in, apt::base *& node) const;
			bool parse_unquoted_text(stream_cursor & in, apt::base *& node) const;
			bool parse_id(stream_cursor & in, apt::text *& node) const;
			bool is_special_character(unsigned char const & c) const;
			bool parse_escaped_text(stream_cursor & in, std::string & unescaped_text) const;
			bool translate_escaped_control_character(unsigned char & value) const;
		public:
			/*
				scan()
				Parses the named input file (infile) and produces the named output
				file (outfile) using the macro environment (e).
			*/
			bool scan(std::string const & infile, std::string const & outfile, env & e);
	};

} // namespace mce

#endif
