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

using namespace v8;

Handle<Value> init_repository(const Arguments& args) {
	
	//initialize the libgit2 thread handling subsystem, will only work if libgit2 was compiled with PTHREAD on
	git_threads_init();

	HandleScope scope;

	Local<String> path = Local<String>::Cast(args[0]);
	Local<Function> callback = Local<Function>::Cast(args[1]);
	const char *err = "";

	git_repository *repo = NULL;

	//AsciiValue flavor overides * operator to return const char
	String::AsciiValue argpath(path);


	//TODO: add support for bare repo argument
        if (git_repository_init(&repo, *argpath,0) < 0) {
		err = "Initilizing repository failed, git_repository_init returned non zero value";
	}

	Local<Value> argv[2] = { Local<Value>::New(String::New(err)),path };
	callback->Call(Context::GetCurrent()->Global(), 2, argv);


	//TODO: are we leaking any memory on the v8 types and related casts/conversions?
        git_repository_free(repo);
        git_threads_shutdown();

	return scope.Close(Undefined());
}

/*
	This function is called from Node to actually register the binding of functions into the Node symbol 'table'. Currently sgit utilizies 
	function (node) to function (c++) to function (c)  mapping as it is most similar to the underlying libgit2 C library.
*/
void init(Handle<Object> exports, Handle<Object> module) {
	exports->Set(String::NewSymbol("init_repository"),FunctionTemplate::New(Method)->GetFunction());

	//alternative form to export as JS constructor rather than individual functions, not needed ATM, kept for reference
	//module->Set(String::NewSymbol("exports"),FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(sgit, init)
