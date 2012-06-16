/*

	guard.h

	Copyright (c) 2012, PARASITE NETWORK AT CYPHER.NU

	GNU General Public License 3

*/

#include <memory>


/*
	Simple pointer guard.
*/

#ifndef guardH_0cd74e28_28cc_4a9d_afbc_009434e40c3d
#define guardH_0cd74e28_28cc_4a9d_afbc_009434e40c3d

template<typename T>
std::unique_ptr<T> make_guard(T * ptr) {
	return std::unique_ptr<T>(ptr);
}

#define GUARD(ptr) auto guarding_##ptr = make_guard(ptr)
#define RELEASE(ptr) guarding_##ptr.release()

#endif

