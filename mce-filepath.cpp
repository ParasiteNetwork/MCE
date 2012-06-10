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

#include "mce-filepath.h"

#include <list>
#include <string>
#include <cstdlib>

#ifndef NOPOSIX
	#include <unistd.h>
#endif

namespace mce {

	namespace /*ANONYMOUS*/ {	
		void split(	std::string const & text, 
					std::string const & separators, 
					std::list<std::string> & words )
		{
			int n = text.length();
			int start, stop;
			start = text.find_first_not_of(separators);
			while ((start >= 0) && (start < n))
		 	{
				stop = text.find_first_of(separators, start);
				if ((stop < 0) || (stop > n)) stop = n;
				words.push_back(text.substr(start, stop - start));
				start = text.find_first_not_of(separators, stop+1);
			}
		}
		
	} // namespace ANONYMOUS
//------------------------------------------------------------------------------	
void filepath::updatecwd(std::string const & path) 
{
	if(path.empty()) return;
	if(path[0] == '/') {
		d_absolute = true;
		d_cwd.clear();
	}
	std::list<std::string> v;
	split(path,"/",v);
	if(v.empty()) return;
	std::list<std::string>::iterator pos = v.begin();
	while(pos != v.end()) {
		if(*pos == "..") {
			if(!d_cwd.empty()) {
				d_cwd.pop_back();
			} else
			if(!d_absolute) {
				d_cwd.push_back(*pos);
			}
		} else
		if(*pos == ".") {
			// ignore
		} else {
			d_cwd.push_back(*pos);
		}
		++pos;
	}
}
//------------------------------------------------------------------------------	
filepath::filepath() : d_absolute(false) 
{
	// updatecwd(".");
}
//------------------------------------------------------------------------------	
filepath::filepath(std::string const & path) : d_absolute(false) 
{
	updatecwd(path);
}
//------------------------------------------------------------------------------
filepath::filepath(filepath const & fp) : d_absolute(fp.d_absolute), d_cwd(fp.d_cwd)
{

}
//------------------------------------------------------------------------------
void filepath::update(filepath const & fp) 
{
	updatecwd(fp.path());
}
//------------------------------------------------------------------------------
void filepath::update(std::string const & path) 
{
	updatecwd(path);
}
//------------------------------------------------------------------------------
void filepath::clear() 
{
	d_absolute = false;
	d_cwd.clear();
}
//------------------------------------------------------------------------------
std::string filepath::path() const 
{
	std::list<std::string>::const_iterator pos = d_cwd.begin();
	std::string np;
	if(d_absolute) np = "/";
	if(d_cwd.empty()) return np;
	np += *(pos++);
	while(pos != d_cwd.end()) {
		np += "/";
		np += *pos;
		++pos;
	}
	return np;
}
//------------------------------------------------------------------------------
std::string filepath::target(std::string const & target) const 
{
	filepath copy(*this);
	copy.update(target);
	return copy.path();
}
//------------------------------------------------------------------------------
bool filepath::absolute() const
{
	return d_absolute;
}
//------------------------------------------------------------------------------
bool filepath::relative() const
{
	return !d_absolute;
}
//------------------------------------------------------------------------------
void filepath::up()
{
	if(!d_cwd.empty()) {
		d_cwd.pop_back();
	}
}
//-STATIC-----------------------------------------------------------------------
std::string filepath::basename(std::string const & path)
{
	std::list<std::string> v;
	split(path,"/",v);
	if(v.empty()) return "";
	return v.back();
}
//-STATIC-----------------------------------------------------------------------
filepath filepath::pathpart(std::string const & path)
{
	filepath fp(path);
	fp.up();
	return fp;
}
//-STATIC-----------------------------------------------------------------------
filepath filepath::home() 
{
	char const * home = std::getenv("HOME");
	if(home) {
		return filepath(home);
	}
	return filepath();
}
//-STATIC-----------------------------------------------------------------------
#ifndef NOPOSIX
filepath filepath::cwd() 
{
	/*
		WARNING: Relying on Linux extension.
	*/
	char * cwd = getcwd(0,0);
	if(cwd) {
		std::string scwd(cwd);
		std::free(cwd);
		return filepath(scwd);
	}
	return filepath();
}
#endif
//------------------------------------------------------------------------------
	
} // namespace mce
	
