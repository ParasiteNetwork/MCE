/*

	mce-scope.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

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
				Top level means that no scopes have been pushed.
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
