/*

	mce-apt.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#ifndef mce_aptH_000b1dd3_60ca_4eec_97e8_c9ed732be5ad
#define mce_aptH_000b1dd3_60ca_4eec_97e8_c9ed732be5ad

#include <vector>
#include <sstream>

#include "forwarding.h"


namespace mce {

	/*
		Forwarding.
	*/
	FORWARD(env)

	/*
		The classes comprising the abstract parse tree.
	*/
	namespace apt {

		/*
			Abstract base class.
		*/
		class base {
			public:
				virtual bool eval(env & e, std::ostringstream & out, bool root = true) const = 0;
				virtual ~base();
		};
		/*
			Definitions.
		*/
		class definable {
			public:
				virtual bool define(env & e) const = 0;
		};
		/*
			Undefinitions.
		*/
		class undefinable {
			public:
				virtual bool undefine(env & e) const = 0;
		};
		/*
			The text class represents all text arguments within the form.
		*/
		class text : public base, public undefinable, public definable {
			protected:
				std::string d_text;
			public:
				std::string const & value() const;
				text(std::string const & text);
				virtual bool eval(env & e, std::ostringstream & out, bool root = true) const;
				virtual bool define(env & e) const;
				virtual bool undefine(env & e) const;
		};
		/*
			Simple binary comparisons are supported by MCE.
			This is the base class.
		*/
		class binop : public base {
			protected:
				base * d_first;
				base * d_second;
			protected:
				bool check_equality(env & e) const;
			public:
				binop(base * first, base * second);
				~binop();
		};
		/*
			Not equal comparison. Default character: #
		*/
		class not_equal : public binop {
			public:
				not_equal(base * first, base * second);
				virtual bool eval(env & e, std::ostringstream & out, bool root = true) const;
		};
		/*
			Equal comparison. Default character: =
		*/
		class equal : public binop, public definable {
			public:
				equal(base * first, base * second);
				virtual bool eval(env & e, std::ostringstream & out, bool root = true) const;
				virtual bool define(env & e) const;
		};
		/*
			The form is the only valid syntax in MCE.

				$(arg, arg, arg, ...)

			The first argument is the callback macro id and will after
			it has been evaluated be used by the environment to find the
			appropriate callback function. 			
		*/
		class form : public base {
			private:
				/*
					The macro id.
				*/
				std::string d_form_macro_id;
				/*
					The arguments.
				*/
				std::vector<base *> d_arguments;
				/*
					Condition to see if a macro id was supplied with the construtor or not.
				*/
				bool d_expecting_macro_id;
			public:
				typedef std::vector<base *>::const_iterator const_iterator;
				/*
					Iteration over the arguments.
				*/
				const_iterator begin() const;
				const_iterator end() const;
				/*
					Returns true if a macro id has been given. Hence the form is callable.
				*/
				bool is_callable() const;
				/*
					Returns true if the form has arguments.
				*/
				bool has_arguments() const;
				/*
					Constructor with explicit macro id. The macro id will otherwise be
					taken from the first argument added to the form.
				*/
				form(text * macro_id);
				/*
					Constructor.	
				*/
				form();
				/*
					
				*/
				virtual bool eval(env & e, std::ostringstream & out, bool root = true) const;
				/*
					Add an argument to the form. If no macro id was supplied with the
					constructor, then the first argument must be a apt::text object.
				*/
				bool add_argument(base * argument);
				/*
					Return the macro id.
				*/
				std::string const & get_macro_id() const;
				/*
					Destructor.
				*/
				~form();
		};
	
	} // namespace apt
} // namespace mce

#endif
