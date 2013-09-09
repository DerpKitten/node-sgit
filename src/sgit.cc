/*
 * The MIT License
 *
 * Copyright (c) 2013 Lush Zero
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "sgit.h"
#include <node.h>
#include <v8.h>
#include <git2.h>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <errno.h>
#include "init.h"
#include "commit.h"
#include "log.h"

using namespace std;

string replaceAll(const string& s, const string& f, const string& r) {
    if (s.empty() || f.empty() || f == r || f.size() > s.size() || s.find(f) == string::npos) {
        return s;
    }
    ostringstream build_it;
    typedef string::const_iterator iter;
    iter i(s.begin());
    const iter::difference_type f_size(distance(f.begin(), f.end()));
    for (iter pos; (pos = search(i , s.end(), f.begin(), f.end())) != s.end(); ) {
        copy(i, pos,  ostreambuf_iterator<char>(build_it));
        copy(r.begin(), r.end(), ostreambuf_iterator<char>(build_it));
        advance(pos, f_size);
        i = pos;
    }
    copy(i, s.end(), ostreambuf_iterator<char>(build_it));
    return build_it.str();
}

using namespace v8;

/*
	This function is called from Node to actually register the binding of functions into the Node symbol 'table'. Currently sgit utilizies 
	function (node) to function (c++) to function (c)  mapping as it is most similar to the underlying libgit2 C library.
*/
void init(Handle<Object> exports, Handle<Object> module) {

	//initialize the libgit2 thread handling subsystem, will only work if libgit2 was compiled with PTHREAD on
	git_threads_init();

	//register init_repository as a module exported function
	exports->Set(String::NewSymbol("init_repository"),FunctionTemplate::New(init_repository)->GetFunction());

	exports->Set(String::NewSymbol("commit_bypath"),FunctionTemplate::New(commit_bypath)->GetFunction());

	exports->Set(String::NewSymbol("log_repository"),FunctionTemplate::New(log_repository)->GetFunction());

	//alternative form to export as JS constructor rather than individual functions, not needed ATM, kept for reference
	//module->Set(String::NewSymbol("exports"),FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(sgit, init)
