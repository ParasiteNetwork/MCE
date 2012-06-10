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
			/*
				There are three types of scopes.
			*/
			enum state {open,closed,deferred};
		public:
			/*
				Pushes a new scope.
			*/
			void push_scope(state s);
			/*
				The same as:
					pop_scope()
					push_scope(s)
			*/
			void change_scope(state s);
			/*
				Pops the current scope.
			*/
			bool pop_scope();
			/*
				Compares the current scope with 's'.
			*/
			bool check_scope(state s) const;
			/*
				Top level means that no scopes has been pushed.
				There is an implicit open scope which cannot be
				popped which is the top level.
			*/
			bool top_level() const;
			/*
				Clears the stack and returns to the top level.
			*/
			void reset();
		private:
			/*
				Stack of scopes.
			*/
			std::list<state> d_scopes;
	};
		
} // namespace mce

#endif
