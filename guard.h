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
