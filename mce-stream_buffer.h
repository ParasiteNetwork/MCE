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


#ifndef mce_stream_bufferH_6c754b19_0eca_4362_a35e_dee6fec82967
#define mce_stream_bufferH_6c754b19_0eca_4362_a35e_dee6fec82967

#include <string>


namespace mce {

	class stream_buffer {
	private:
		std::string d_buffer;
		unsigned int d_buffer_length;
	public:
		stream_buffer();
		bool open(std::string const & file);
		bool get(unsigned int const index, unsigned char & c) const;
		bool compare(unsigned int const index, unsigned char const & c) const;
		bool is_valid_index(unsigned int index) const;
	};

	class stream_cursor {
	private:
		/*
			Absolute index within the buffer.
		*/
		unsigned int d_index;
		/*
			Index where the current row starts.
		*/
		unsigned int d_start_of_current_row;
		/*
			Number of rows encountered.
		*/
		unsigned int d_row_counter;
		/*
			Relative index in the current row.
		*/
		unsigned int d_row_index;
		stream_buffer const & d_buffer;
	public:
		bool match(unsigned char const & c) const;
		bool accept(unsigned char const & c);
		bool match(std::string const & text) const;
		bool accept(std::string const & text);
		bool advance();
		bool good() const;
		unsigned char current() const;
		stream_cursor(stream_cursor const & other);
		stream_cursor(stream_buffer const & buffer);
		void operator=(stream_cursor const & other);
		std::string const get_current_row(unsigned char const & delim = '\n') const;
		std::string const get_current_partial_row() const;
		unsigned int get_row_number() const;
		unsigned int get_caret() const;
	};

} /* namespace mce */

#endif