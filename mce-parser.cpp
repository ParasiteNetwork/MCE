/*

	mce-parser.cpp

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

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

template<typename T>
typename T::iterator last(T & v) {
	typename T::iterator ii = v.end();
	return --ii;
}

//------------------------------------------------------------------------------
/*
	Initializes the parser and calls parse_stream().
*/	
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
	/*
		Clear session states.
	*/
	d_env = &e;
	d_env->get_scope().reset();
	d_intend.clear();
	/*
		Parse.
	*/
	stream_cursor cursor(buffer);
	return parse_stream(cursor);
}
//------------------------------------------------------------------------------
/*
	Removes spaces and tabs from the stream and returns its condition.
*/
bool parser::trim(stream_cursor & in) const
{
	while(in.good() && 
		(in.accept(vars::mce_tab_char) || 
			in.accept(vars::mce_space_char)));
	return in.good();
}
//------------------------------------------------------------------------------
/*
	Prints a single character to the output file.
*/
void parser::print(unsigned char const & c)
{
	d_out << c;
}
//------------------------------------------------------------------------------
/*
	Prints a macro expansion.
	Since a macro can expand into text that spans over multiple lines, we keep
	track of where in the file the expansion occurs; this is the indentation
	level. The indentation is calculated in parse_stream(). The first line
	is printed at the current position, but all subsequent lines are prepended
	by the indentation to line them up.
	For example (where \_ is an escape for space):

		\_\_\t$(M)

	where M expands to "1\n2\3" will be printed as

		"1\n"
		"\_\_\t2\n"
		"\_\_\t3"
*/
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
/*
	It simply copies each character from the input file to the output file.
	But while doing so it looks for the macro escape character.
*/
bool parser::parse_stream(stream_cursor & in) 
{
	while(in.good()) {
		/*
			Check if the character is an escape character.
		*/
		if(check_macro_escape_character(in)) {
			apt::base * root_node = 0;
			GUARD(root_node);
			/*
				We localize the stream cursor to protect it.
			*/
			stream_cursor localin(in);
			/*
				Now we try to parse the macro into an abstract parse tree. 
			*/
			if(parse_macro(localin, root_node)) {
				std::ostringstream out;
				/*
					Only if the evaluation succeeds to be print the expansion.
				*/
				if(root_node->eval(*d_env,out,true)) {
					print(out.str());
					in = localin;
					/*
						Since we have expanded a macro we'll skip the default
						copying below.
					*/
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
/*
	Checks if the current character is an escape character.
*/
bool parser::check_macro_escape_character(stream_cursor & in) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	return in.match(vars::mce_escape_char);
}
//------------------------------------------------------------------------------
/*
	Tries to parse the escape character.
*/
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
		The macro form consist of a callback shaped like list between two
		parentheses. The first element is the callback name, followed by any
		arguments separated with commas:

			(id, arg, arg)

		Multiple commas are collapsed:

			(id, ,, arg,, , arg,,)

		A callback without arguments may use the syntactic suger:

			$id

		Which is the same as:

			$(id)

		Arguments are text strings, even if they are supplied as just numbers.
		Without enclosing double qoutes, escape characters are disallowed,
		spaces are collapsed and trailing spaces are removed. With double
		qoutes everything is as it is, and escape characters are translated.

			$(id, some text , "yet more \n text",, 9)

		Nested:

			$(print, $(concatenate, hello, " world", $newline))

		This function is recursively called to parse nested macro forms.
	*/
	stream_cursor localin(in);

	if(!parse_macro_escape_character(localin)) {
		return false;
	}

	if(!parse_macro_proper_form(localin, node)) {
		if(!parse_macro_syntactic_suger_form(localin, node)) {
			return false;
		}
	}
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
/*
	Parses proper forms, i.e. arguments enclosed between parenthesis.
	$(proper, macro, form)
*/
bool parser::parse_macro_proper_form(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	/*
		Make sure the form begins with an opening parenthesis.
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
		it just means that there were trailing commas in the macro form
		before the call, which is perfectly valid, i.e.

			(id,,,,)

		We do not allow empty forms. 
		And the callback must be known at parse time.
	*/
	while(parse_macro_argument(localin, argument)) {
		/*
			The first argument is the macro id. If it's written wrong
			then add_argument() will fail.
		*/
		if(!form->add_argument(argument)) {
			debugf("MCE(parser::parse_macro_proper_form): Invalid form argument!");
			debugf("MCE(parser::parse_macro_proper_form): Most likely the macro id is wrongly specified!");
			debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", 
				localin.get_row_number(), localin.get_current_row().c_str());
			return false;
		}
		/*
			Clean up spaces, tabs and commas.
		*/
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
		Make sure it is callable, which means that it has at least an id.
	*/
	if(!form->is_callable()) {
		debugf("MCE(parser::parse_macro_proper_form): Empty forms are not allowed!");
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", 
			localin.get_row_number(), localin.get_current_partial_row().c_str());
		return false;
	}
	/*
		Make sure the callback is known.
	*/
	if(!validate_callback(form->get_macro_id())) {
		debugf("MCE(parser::parse_macro_proper_form): The callback must be known!");
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", 
			localin.get_row_number(), localin.get_current_partial_row().c_str());
		debugf("MCE(parser::parse_macro_proper_form): '%s' is unkown.", 
			form->get_macro_id().c_str());
		return false;
	}
	/*
		Make sure there is an ending parenthesis.
	*/
	if(!localin.accept(vars::mce_scope_close_char)) {
		debugf("MCE(parser::parse_macro_proper_form): Expected a closing '%c'!", 
			vars::mce_scope_close_char);
		debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", 
			localin.get_row_number(), localin.get_current_partial_row().c_str());
		return false;
	}
	in = localin;
	node = RELEASE(form);
	return true;
}
//------------------------------------------------------------------------------
/*
	Parses macro arguments. 
*/
bool parser::parse_macro_argument(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	if(!trim(localin)) {
		return false;
	}
	/*
		First we see if the argument is simply either a qouted or uqouted
		text argument. If not, we try to parse it as a macro form.
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
	/*
		Finally we parse any comparison bound to the argument.
	*/
	if(!parse_binop(localin,node)) {
		return false;
	}
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
/*
	Macro forms without any arguments may be written in a shortened version:

		$(noargs)

	is the same as:

		$noargs

	The restriction is that the macro id can not be made up of multiple words:

		$(no args)

	is not valid as:

		$no args

	as only $no will be recognized.
*/
bool parser::parse_macro_syntactic_suger_form(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	/*
		We just need the macro id, and then we create an ordinary macro form.
	*/
	apt::text * id = 0;
	GUARD(id);
	if(parse_id(localin,id)) {
		/*
			Make sure the callback is known. This is a copy of the same restriction
			in parse_macro_proper_form().
		*/
		if(!validate_callback(id->value())) {
			debugf("MCE(parser::parse_macro_proper_form): The callback must be known!");
			debugf("MCE(parser::parse_macro_proper_form): @%i, '%s'", localin.get_row_number(), localin.get_current_partial_row().c_str());
			debugf("MCE(parser::parse_macro_proper_form): '%s' is unkown.", id->value().c_str());
			return false;
		}
		node = new apt::form(RELEASE(id));
		in = localin;
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
/*
	Arguments can be compared with other arguments.

		$(id, me = you)
		$(id, me # you)

	The macro id can not be compared, only arguments.
*/
bool parser::parse_binop(stream_cursor & in, apt::base *& node) const
{
	stream_cursor localin(in);
	if(!trim(localin)) {
		return false;
	}
	/*
		Only mce_equal and mce_not_equal are legal in a comparison context.
		mce_equal being '=' and mce_not_equal being '#'.
	*/
	apt::base * part = 0;
	if(!localin.match(vars::mce_equal_char) && !localin.match(vars::mce_not_equal_char)) {
		/*
			Yes, we fail with a happy face. It isn't actually a failure. Since no
			comparison character has been found, it is not a parse error.
		*/
		return true;
	}
	unsigned char comparison_char = localin.current();
	localin.advance();
	if(!trim(localin)) {
		return false;
	}
	/*
		We need the argument compared with.
	*/
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
		debugf("MCE(parser::parse_binop): Should have '%c' or '%c', but have instead '%c'!?",
			vars::mce_equal_char, vars::mce_not_equal_char, comparison_char);
		debugf("MCE(parser::parse_binop): This should not be possible. FATAL INTERNAL ERROR!");
		return false;
	}
	in = localin;
	return true;
}
//------------------------------------------------------------------------------
/*
	Checks using the macro environment if the macro id is defined.
*/
bool parser::validate_callback(std::string const & id) const
{
	if(d_env->is_defined(id)) {
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
/*
	Removes spaces, tabs and commas. Returns the condition of the stream.
*/
bool parser::trim_commas(stream_cursor & in) const
{
	/*
		Localized stream_cursor not necessary. No backtracking is performed.
	*/
	while(trim(in)) {
		if(!in.accept(vars::mce_comma_char)) {
			break;
		}
	}
	return in.good();
}
//------------------------------------------------------------------------------
/*
	Parses qouted text. Anything is allowed between the qoutes.
*/
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
/*
	Parses unqouted text.
*/
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
		Delete any trailing space.
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
/*
	Parses a macro id within the context of syntactic suger forms. It is almost
	the same as unqouted text, but spaces are not allowed, hence disallowing
	multiple words.
*/
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
/*
	Returns true if the character used by the MCE parser.
*/
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
/*
	Parses escape sequences and translates them.
*/
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
/*
	Translates escaped control characters.
*/
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
