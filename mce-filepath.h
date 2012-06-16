/*

	mce-filepath.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/


#ifndef mce_filepathH_c035d76d_6a31_4140_af8c_2980fb929938
#define mce_filepathH_c035d76d_6a31_4140_af8c_2980fb929938

#include <string>
#include <list>
#include <ostream>


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
