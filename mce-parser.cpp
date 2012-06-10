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

#include "mce-parser.h"
#include "mce-vars.h"
#include "mce-stream_buffer.h"
#include "mce-apt.h"
#include "mce-env.h"
#include "guard.h"
#include "debug.h"
#include "basic_latin.h"


namespace mce {

//------------------------------------------------------------------------------
namespace 
{
	template<typename T>
	typename T::iterator last(T & v) {
		typename T::iterator ii = v.end();
		return --ii;
	}
	template<typename T>
	void destroy(T & v) {
		typename T::iterator pos = v.begin();
		while(pos != v.end()) {
			delete *pos;
			++pos;
		}
	}
}
//------------------------------------------------------------------------------	
bool parser::scan(std::string const & infile, std::string const & outfile, env & e) 
{
	/*
		Create the stream buffer.
	*/
	stream_buffer buffer;
	if(!buffer.open(infile)) {
		debugf("MCE(parser::scan): Source file %s could not be opened!",infile.c_str());
		return false;
	}
	/*
		Open the output file.
	*/
	d_out.open(outfile.c_str());
	if(!d_out) {
		debugf("MCE(parser::scan): Output file %s could not be opened!",outfile.c_str());
		return false;
	}
	d_env = &e;
	d_env->get_scope().reset();
	d_intend.clear();
	/*
		Proceed to parse the stream through a cursor.
	*/
	stream_cursor cursor(buffer);
	d_env->get_scope().reset();
	return parse_stream(cursor);
}
//------------------------------------------------------------------------------
bool parser::trim(stream_cursor & in) const
{
	/*
		This function simply removes tabs and spaces and returns the
		condition of the stream.
	*/
	while(in.good() && 
		(in.accept(vars::mce_tab_char) || 
			in.accept(vars::mce_space_char)));
	return in.good();
}
//------------------------------------------------------------------------------
void parser::print(unsigned char const & c)
{
	d_out << c;
}
//------------------------------------------------------------------------------
void parser::print(std::string const & text)
{
	std::istringstream in(text);
	std::string line;
	bool do_intendation = false;
	while(std::getline(in,line)) {
		if(do_intendation) {
			d_out << "\n";
			d_out << d_intend;
		}
		d_out << line;
		do_intendation = true;
	}
}
//------------------------------------------------------------------------------
bool parser::parse_stream(stream_cursor & in) 
{
	/*
		This loop will copy each character from the input to the output. Any
		macros encountered will be expanded and its expansion will be printed
		to the output. Control statements can temporarily close the scope and
		thus prevent some text from being copied. 
	*/
	while(in.good()) {
		if(check_macro_escape_character(in)) {
			/*
				Parse the macro into an abstract parse tree.
			*/
			apt::base * root_node = 0;
			GUARD(root_node);
			stream_cursor localin(in);
			if(parse_macro(localin, root_node)) {
				std::ostringstream out;
				if(root_node->eval(*d_env,out,true)) {
					print(out.str());
					in = localin;
					continue;
				}
			}
		}

		/*
			If the scope isn't open we ignore the current character.
		*/
		if(d_env->get_scope().check_scope(scope::open)) {
			/*
				We adjust the intendation level according to the input.
				A tab increases it with a tab. A newline clears the intendation,
				and any other character increases it with a single space.
				We perform this tracking so we can care for the proper
				intendation level when a macro expands into text that spills
				over multiple lines.
			*/
			if(in.match(vars::mce_tab_char)) {
				d_intend += vars::mce_tab_char;
			} else
			if(in.match(basic_latin::control::c_newline)) {
				d_intend.clear();
			} else {
				d_intend += vars::mce_space_char;
			}
			/*
				Print the current character and move on to the next.
			*/
			print(in.current());
			
		}
		in.advance();
	}
	return true;
}
//------------------------------------------------------------------------------
bool parser::check_macro_escape_character(stream_cursor & in) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	return in.match(vars::mce_escape_char);
}
//------------------------------------------------------------------------------
bool parser::parse_macro_escape_character(stream_cursor & in) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	return in.accept(vars::mce_escape_char);
}
//------------------------------------------------------------------------------
bool parser::parse_macro(stream_cursor & in, apt::base *& node) const
{
	/*
		The macro form consist of a callback shaped like list. Between two
		paranthesis the first element is the callback name, followed by any
		arguments separated with commas:

			(id, arg, arg)

		Multiple commas are collapsed:

			(id, ,, arg,, , arg,,)

		A callback without arguments may use the syntactic suger:

			$id

		Which is the same as:

			(id)

		Arguments are text strings, even if they are supplied as just numbers.
		Without enclosing double qoutes, escape characters are disallowed,
		spaces are collapsed and trailing spaces are removed. With double
		qoutes everything is as it is, and escape characters are translated.

			(id, some text , "yet more \n text",, 9)

		Nested:

			(print, (concatenate, hello, " world", $newline))

		This function is recursively called to parse nested macro forms.
	*/
	stream_cursor localin(in);

	if(!parse_macro_escape_character(localin)) {
		return false;
	}
	/*
		If the current character is a opening paranthesis, we'll assume we're
		expecting a proper macro form. I.e.

			(id, arg, arg)
	*/
	/*
		Otherwise, if we find an escape character, it looks like there is
		a syntactic suger macro form ahead. I.e.

			$id
	*/
	if(!parse_macro_proper_form(localin, node)) {
		if(!parse_macro_syntactic_suger_form(localin, node)) {
			return false;
		}
	}
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
bool parser::parse_macro_proper_form(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	/*
		Make sure the form starts with a opening paranthesis.
	*/
	if(!localin.accept(vars::mce_scope_open_char)) {
		return false;
	}
	apt::form * form = new apt::form;
	apt::base * argument = 0;
	GUARD(form);
	/*
		We loop and parse each argument in turn, until there are no more
		valid arguments. We issue a preemptive break if there is no comma
		after an argument. If a failed parse_macro_argument() ends the loop
		it just means that there was trailing commas in the macro form, which
		is perfectly valid. I.e.

			(id,,,,)

		We do not allow empty forms. 
		And the callback must be known at parse time.
	*/
	while(parse_macro_argument(localin, argument)) {
		if(!form->add_argument(argument)) {
			debugf("MCE(parser::parse_macro_proper_form): Invalid form argument!");
			debugf("MCE(parser::parse_macro_proper_form): Most likely the macro id is wrongly specified!");
			debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_row().c_str());
			return false;
		}
		if(!trim(localin)) {
			break;
		}
		if(!localin.accept(vars::mce_comma_char)) {
			break;
		}
		trim_commas(localin);
	}
	/*
		RESTRICTIONS.
	*/
	/*
		Make sure it isn't empty.
	*/
	if(!form->is_callable()) {
		debugf("MCE(parser::parse_macro_proper_form): Empty forms are not allowed!");
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_partial_row().c_str());
		return false;
	}
	/*
		Make sure the callback is known.
	*/
	if(!validate_callback(form->get_macro_id())) {
		debugf("MCE(parser::parse_macro_proper_form): The callback must be known!");
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_partial_row().c_str());
		debugf("MCE(parser::parse_macro_proper_form): '%s' is unkown.", form->get_macro_id().c_str());
		return false;
	}
	/*
		Make sure there is an ending paranthesis.
	*/
	if(!localin.accept(vars::mce_scope_close_char)) {
		debugf("MCE(parser::parse_macro_proper_form): Expected a closing '%c'!", vars::mce_scope_close_char);
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_partial_row().c_str());
		return false;
	}
	in = localin;
	node = RELEASE(form);
	return true;
}
//------------------------------------------------------------------------------
bool parser::parse_macro_argument(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	if(!trim(localin)) {
		return false;
	}
	/*
		First we try to parse the argument as either an unqouted or a qouted 
		text. If that fails, we try to see if it is a nested macro form.
		Then we parse any equality operator tied to the argument.

			$(if, $(employee no, 6) = Anna Karlsson)
	*/
	if(basic_latin::is_basic_alpha(localin.current())) {
		if(!parse_unquoted_text(localin,node)) {
			return false;
		}
	} else
	if(localin.match(vars::mce_text_mark_char)) {
		if(!parse_quoted_text(localin,node)) {
			return false;
		}
	} else
	if(!parse_macro(localin,node)) {
		return false;
	}

	if(!parse_binop(localin,node)) {
		return false;
	}
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
bool parser::parse_macro_syntactic_suger_form(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	/*
		We just need the macro id, and then we create an ordinary macro form.
	*/
	apt::text * id = 0;
	if(parse_id(localin,id)) {
		/*
			Make sure the callback is known.
		*/
		if(!validate_callback(id->value())) {
			debugf("MCE(parser::parse_macro_proper_form): The callback must be known!");
			debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_partial_row().c_str());
			debugf("MCE(parser::parse_macro_proper_form): '%s' is unkown.", id->value().c_str());
			delete id;
			return false;
		}
		node = new apt::form(id);
		in = localin;
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool parser::parse_binop(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	if(!trim(localin)) {
		return false;
	}
	/*
		Only mce_equal and mce_not_equal are legal in a comparison context.
	*/
	apt::base * part = 0;
	if(!localin.match(vars::mce_equal_char) && !localin.match(vars::mce_not_equal_char)) {
		/*
			Yes, we fail with a true value.
		*/
		return true;
	}
	unsigned char comparison_char = localin.current();
	localin.advance();
	if(!trim(localin)) {
		return false;
	}
	if(!parse_macro_argument(localin,part)) {
		debugf("MCE(parser::parse_binop): Compare with what?");
		debugf("MCE(parser::parse_binop): Saw '%c' but never the right hand.");
		debugf("MCE(parser::parse_binop): %i : %s", localin.get_row_number(), localin.get_current_row().c_str());
		return false;
	}
	if(comparison_char == vars::mce_equal_char) {
		node = new apt::equal(node,part);
	} else
	if(comparison_char == vars::mce_not_equal_char) {
		node = new apt::not_equal(node,part);
	} else {
		debugf("MCE(parser::parse_binop): This should not print.");
		return false;
	}
	/*
	switch(comparison_char) {
		case vars::mce_equal_char:
			node = new apt::equal(node,part);
			break;
		case vars::mce_not_equal_char:
			node = new apt::not_equal(node,part);
			break;
	}
	*/
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
bool parser::validate_callback(std::string const & id) const
{
	if(d_env->is_defined(id)) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool parser::trim_commas(stream_cursor & in) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	/*
		Removes any spaces and commas (empty components).
	*/
	while(trim(in)) {
		if(!in.accept(vars::mce_comma_char)) {
			break;
		}
	}
	return in.good();
}
//------------------------------------------------------------------------------
bool parser::parse_quoted_text(stream_cursor & in, apt::base *& node) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	std::string parsed_text;
	if(!in.accept(vars::mce_text_mark_char)) {
		return false;
	}
	while(in.good()) {
		if(in.match(vars::mce_text_mark_char)) {
			break;
		}
		if(in.match('\\')) {
			/*
				Parse and translate the escaped sequence.
			*/
			std::string unescaped_text;
			if(parse_escaped_text(in,unescaped_text)) {
				parsed_text += unescaped_text;
				continue;
			}
		}
		parsed_text += in.current();
		in.advance();
	}
	if(!in.accept(vars::mce_text_mark_char)) {
		return false;
	}
	if(!parsed_text.empty()) {
		node = new apt::text(parsed_text);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool parser::parse_unquoted_text(stream_cursor & in, apt::base *& node) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	std::string parsed_text;
	while(in.good()) {
		/*
			Control characters are not allowed.
		*/
		if(basic_latin::is_control(in.current())) {
			break;
		}
		/*
			The character set used by the MCE parser is not allowed.
		*/
		if(is_special_character(in.current())) {
			break;
		}
		/*
			Consecutive spaces and tabs are collapsed into one space.
		*/
		if(basic_latin::is_spacing(in.current())) {
			if(!parsed_text.empty()) {
				if(!basic_latin::is_spacing(*last(parsed_text))) {
					parsed_text += vars::mce_space_char;
				}
			} else {
				parsed_text += vars::mce_space_char;
			}
		}
		else {
			/*
				Accept anything else. This also includes the entire unicode
				universe outside basic latin. Even if they are not really
				printable characters, this implementation doesn't really
				care right now.
			*/
			parsed_text += in.current();
		}
		in.advance();
	}
	/*
		Delete a trailing space.
	*/
	if(basic_latin::is_spacing(*last(parsed_text))) {
		std::string::size_type length = parsed_text.length();
		parsed_text = parsed_text.substr(0,length - 1);
	}
	if(!parsed_text.empty()) {
		node = new apt::text(parsed_text);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool parser::parse_id(stream_cursor & in, apt::text *& node) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	std::string parsed_text;
	while(in.good()) {
		/*
			Control characters are not allowed.
		*/
		if(basic_latin::is_control(in.current())) {
			break;
		}
		/*
			The character set used by the MCE parser is not allowed.
		*/
		if(is_special_character(in.current())) {
			break;
		}
		/*
			Space and tab is not allowed.
		*/
		if(basic_latin::is_spacing(in.current())) {
			break;
		}
		parsed_text += in.current();
		in.advance();
	}
	if(!parsed_text.empty()) {
		node = new apt::text(parsed_text);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool parser::is_special_character(unsigned char const & c) const
{
	if(c == vars::mce_escape_char) return true;
	if(c == vars::mce_scope_open_char) return true;
	if(c == vars::mce_scope_close_char) return true;
	if(c == vars::mce_comma_char) return true;
	if(c == vars::mce_equal_char) return true;
	if(c == vars::mce_not_equal_char) return true;
	if(c == vars::mce_text_mark_char) return true;
	return false;
}
//------------------------------------------------------------------------------
bool parser::parse_escaped_text(stream_cursor & in, std::string & unescaped_text) const
{
	stream_cursor localin(in);
	if(!localin.accept('\\') || !localin.good()) {
		return false;
	}
	if(localin.accept('^') && localin.good()) {
		unsigned char c = localin.current();
		if(translate_escaped_control_character(c)) {
			unescaped_text += c;
		} else {
			return false;
		}
	} else {
		switch(localin.current()) {
			case 't': unescaped_text += '\t'; break;
			case 'n': unescaped_text += '\n'; break;
			case 'r': unescaped_text += '\r'; break;
			case 'f': unescaped_text += '\f'; break;
			case '_': unescaped_text += ' '; break;
			//case 'N': unescaped_text += basic_latin::control::next_line; // NEL
			// case 'P': return 0x2029; // PS
			// case 'L': return 0x2028; // LS
			/*
				Just accept it as it is.
			*/
			default: unescaped_text += localin.current();
		}
	}
	localin.advance();
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
bool parser::translate_escaped_control_character(unsigned char & value) const
{
	switch(value) {
		case '0': return '\x0';
		case 'A': case 'a': value = '\x1'; break;
		case 'B': case 'b': value = '\x2'; break;
		case 'C': case 'c': value = '\x3'; break;
		case 'D': case 'd': value = '\x4'; break;
		case 'E': case 'e': value = '\x5'; break;
		case 'F': case 'f': value = '\x6'; break;
		case 'G': case 'g': value = '\x7'; break;
		case 'H': case 'h': value = '\x8'; break;
		case 'I': case 'i': value = '\x9'; break;
		case 'J': case 'j': value = '\xA'; break;
		case 'K': case 'k': value = '\xB'; break;
		case 'L': case 'l': value = '\xC'; break;
		case 'M': case 'm': value = '\xD'; break;
		case 'N': case 'n': value = '\xE'; break;
		case 'O': case 'o': value = '\xF'; break;
		case 'P': case 'p': value = '\x10'; break;
		case 'Q': case 'q': value = '\x11'; break;
		case 'R': case 'r': value = '\x12'; break;
		case 'S': case 's': value = '\x13'; break;
		case 'T': case 't': value = '\x14'; break;
		case 'U': case 'u': value = '\x15'; break;
		case 'V': case 'v': value = '\x16'; break;
		case 'W': case 'w': value = '\x17'; break;
		case 'X': case 'x': value = '\x18'; break;
		case 'Y': case 'y': value = '\x19'; break;
		case 'Z': case 'z': value = '\x1A'; break;
		case '[': value = '\x1B'; break;
		case '\\': value = '\x1C'; break;
		case ']': value = '\x1D'; break;
		case '^': value = '\x1E'; break;
		case '_': value = '\x1F'; break;
		case '~': value = '\x7F'; break;
		default: return false;
	}
	return true;
}
//------------------------------------------------------------------------------

} // namespace mce
