/*
Copyright (c) 2008, Tobias Blomkvist <projects.tracer@cypher.nu>

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

#ifndef mce_filepathH_c035d76d_6a31_4140_af8c_2980fb929938
#define mce_filepathH_c035d76d_6a31_4140_af8c_2980fb929938

#include <string>
#include <list>
#include <ostream>

// foo
// bar/foo
// /bar/foo
// bar/foo/

namespace mce {

	class filepath {
		private:
			bool d_absolute;
			std::list<std::string> d_cwd;
		private:
			void updatecwd(std::string const & path);	
		public:
			filepath();
			filepath(filepath const & fp);
			filepath(std::string const & path);
			void update(filepath const & fp);
			void update(std::string const & path);
			void clear();
			std::string path() const;
			std::string target(std::string const & target) const;
			bool absolute() const;
			bool relative() const;
			void up();
			
			static std::string basename(std::string const & path);
			static filepath pathpart(std::string const & path);
			static filepath home();
#ifndef NOPOSIX
			static filepath cwd();
#endif
			friend std::ostream & operator<<(std::ostream & out, filepath const & fp) {
				return out << fp.path();
			}

	};

} // namespace mce

#endif
