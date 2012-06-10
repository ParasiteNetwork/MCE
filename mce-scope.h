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


#ifndef mce_scopeH_4a42bbe3_7b91_4859_89d9_19cd5f8d9754
#define mce_scopeH_4a42bbe3_7b91_4859_89d9_19cd5f8d9754

#include <list>


namespace mce {

	class scope {
		public:
			enum state {open,closed,deferred};
		public:
			void push_scope(state s);
			void change_scope(state s);
			bool pop_scope();
			bool check_scope(state s) const;
			bool top_level() const;
			void reset();
			
			~scope();
		private:
			std::list<state> d_scopes;
	};
		
} // namespace mce

#endif
