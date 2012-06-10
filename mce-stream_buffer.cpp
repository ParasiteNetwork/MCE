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

#include "mce-stream_buffer.h"
//#include "debug.h"

#include <cstddef>
#include <string>
#include <fstream>
#include <sstream>

namespace mce 
{

// STREAM_BUFFER
//------------------------------------------------------------------------------
stream_buffer::stream_buffer()
{

}
//------------------------------------------------------------------------------
bool stream_buffer::open(std::string const & file)
{
	std::ifstream in(file.c_str());
	if(!in) {
		return false;
	}
	std::stringstream temp_buffer;
	temp_buffer << in.rdbuf();
	d_buffer = temp_buffer.str();
	d_buffer_length = d_buffer.size();
	return true;
}
//------------------------------------------------------------------------------
bool stream_buffer::get(unsigned int index, unsigned char & c) const
{
	if(index < d_buffer_length) {
		c = d_buffer[index];
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------
bool stream_buffer::compare(unsigned int index, unsigned char const & c) const
{
	if(index < d_buffer_length) {
		return d_buffer[index] == c;
	}
	return false;
}
//------------------------------------------------------------------------------
bool stream_buffer::is_valid_index(unsigned int index) const
{
	return index < d_buffer_length;
}
//------------------------------------------------------------------------------
// STREAM_CURSOR
//------------------------------------------------------------------------------
stream_cursor::stream_cursor(stream_buffer const & buffer) : d_buffer(buffer)
{
	d_index = 0;
	d_start_of_current_row = 0;
	d_row_counter = 1;
	d_row_index = 0;
}
//------------------------------------------------------------------------------
stream_cursor::stream_cursor(stream_cursor const & cursor) : d_buffer(cursor.d_buffer)
{
	*this = cursor;
}
//------------------------------------------------------------------------------
void stream_cursor::operator=(stream_cursor const & cursor)
{
	d_index = cursor.d_index;
	d_start_of_current_row = cursor.d_start_of_current_row;
	d_row_counter = cursor.d_row_counter;
	d_row_index = cursor.d_row_index;
}
//------------------------------------------------------------------------------
bool stream_cursor::match(unsigned char const & c) const
{
	return d_buffer.compare(d_index,c);
}
//------------------------------------------------------------------------------
bool stream_cursor::accept(unsigned char const & c)
{
	// debugf("stream_cursor::accept '%c' at %u where '%c'", c, d_index,current());
	if(d_buffer.compare(d_index,c)) {
		advance();
		// debugf("	stream_cursor::accept index now %u", d_index);
		return true;
	}
	// debugf("	stream_cursor::accept failed");
	return false;
}
//------------------------------------------------------------------------------
bool stream_cursor::match(std::string const & text) const
{
	stream_cursor local(*this);
	for(std::size_t i = 0; i < text.size(); ++i) {
		if(!local.accept(text[i])) {
			return false;
		}
	}
	return true;
}
//------------------------------------------------------------------------------
bool stream_cursor::accept(std::string const & text)
{
	stream_cursor local(*this);
	for(std::size_t  i = 0; i < text.size(); ++i) {
		if(!local.accept(text[i])) {
			return false;
		}
	}
	*this = local;
	return true;
}
//------------------------------------------------------------------------------
bool stream_cursor::advance()
{
	++d_row_index;
	if(d_buffer.compare(d_index,'\n')) {
		++d_row_counter;
		d_row_index = 0;
		d_start_of_current_row = d_index + 1;
	}

	return d_buffer.is_valid_index(++d_index);
}
//------------------------------------------------------------------------------
std::string const stream_cursor::get_current_row(unsigned char const & delim) const
{
	std::string row_text;
	stream_cursor local(*this);
	local.d_index = d_start_of_current_row;
	while(!local.accept(delim) && local.good()) {
		row_text += local.current();
		local.advance();
	}
	return row_text;
}
//------------------------------------------------------------------------------
std::string const stream_cursor::get_current_partial_row() const
{
	std::string row_text;
	for(unsigned int pos = d_start_of_current_row; pos < d_index; ++pos) {
		unsigned char c = 0xFF;
		d_buffer.get(pos,c);
		row_text += c;
	}
	return row_text;
}
//------------------------------------------------------------------------------
bool stream_cursor::good() const
{
	return d_buffer.is_valid_index(d_index);
}
//------------------------------------------------------------------------------
unsigned char stream_cursor::current() const
{
	unsigned char c = 0xFF;
	d_buffer.get(d_index,c);
	return c;	
}
//------------------------------------------------------------------------------
unsigned int stream_cursor::get_row_number() const
{
	return d_row_counter;
}
//------------------------------------------------------------------------------
unsigned int stream_cursor::get_caret() const
{
	return d_row_index;
}
//------------------------------------------------------------------------------
} // namespace mce