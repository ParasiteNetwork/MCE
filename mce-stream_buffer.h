/*

	mce-stream_buffer.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#ifndef mce_stream_bufferH_6c754b19_0eca_4362_a35e_dee6fec82967
#define mce_stream_bufferH_6c754b19_0eca_4362_a35e_dee6fec82967

#include <string>

#include "forwarding.h"

namespace mce {

	/*
		Forwarding.
	*/
	FORWARD(stream_cursor)

	/*
		The stream_buffer class reads the entire content of a file into
		a buffer. A stream_cursor can then navigate it.
	*/
	class stream_buffer {
		friend class stream_cursor;
		private:
			/*
				The buffer.
			*/
			std::string d_buffer;
			/*
				Basically the same as d_buffer.size(). Just one indirection less,
				and probably not really necessary.
			*/
			unsigned int d_buffer_length;
		public:
			/*
				Constructor.
			*/
			stream_buffer();
			/*
				Opens a file and reads the entire content into the buffer.
			*/
			bool open(std::string const & file);
		private:
			/*
				Returns (by reference parameter) the character at the index.
			*/
			bool get(unsigned int const index, unsigned char & c) const;
			/*
				Compares the character with the buffer character at the index.
			*/
			bool compare(unsigned int const index, unsigned char const & c) const;
			/*
				Bounds checking.
			*/
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
			/*
				Reference to the stream buffer.
			*/
			stream_buffer const & d_buffer;
		public:
			/*
				Returns true if the current character matches 'c'.
			*/
			bool match(unsigned char const & c) const;
			/*
				Same as match(), but also moves the stream cursor
				forward on success.
			*/
			bool accept(unsigned char const & c);
			/*
				Matches a text string.
			*/
			bool match(std::string const & text) const;
			/*
				Accepts a text string.
			*/
			bool accept(std::string const & text);
			/*
				Moves the stream cursor forward.
			*/
			bool advance();
			/*
				Returns true if the stream has not reached EOS.
			*/
			bool good() const;
			/*
				Returns the current character.
			*/
			unsigned char current() const;
			/*
				Copy constructor.
			*/
			stream_cursor(stream_cursor const & other);
			/*
				Constructor. 
				Needs a stream buffer to operate on.
			*/
			stream_cursor(stream_buffer const & buffer);
			/*
				Assignment operator.
			*/
			void operator=(stream_cursor const & other);
			/*
				Returns the current row.
			*/
			std::string const get_current_row(unsigned char const & delim = '\n') const;
			/*
				Returns the current row upto where we are.
			*/
			std::string const get_current_partial_row() const;
			/*
				Returns the current row number.
			*/
			unsigned int get_row_number() const;
			/*
				Returns where we are on the current row. Local index.
			*/
			unsigned int get_caret() const;
	};

} /* namespace mce */

#endif