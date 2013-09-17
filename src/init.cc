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
#include "init.h"

using namespace v8;

struct init_params
{
	char* path;
	const char* err;
	Persistent<Function> callback;
};

void init_repository_worker(uv_work_t* req) {

	
	//git_threads_init();
	init_params* request = (init_params*)req->data;
	git_repository *repo = NULL;
	const char* err = "";
	if (git_repository_init(&repo, request->path,0) < 0) {
		//const git_error *error = giterr_last();

		err = "Initilizing repository failed, git_repository_init returned non zero value";
        }

	request->err = err;
	git_repository_free(repo);
	//git_threads_shutdown();
}

void init_repository_callback(uv_work_t* req) {

	HandleScope scope;

	init_params* request = (init_params*)req->data;
	delete req;

	Handle<Value> argv[1] = { Local<Value>::New(String::New(request->err)) };

	request->callback->Call(Context::GetCurrent()->Global(), 1, argv);

    	request->callback.Dispose();
    	delete request;

}

extern "C" Handle<Value> init_repository(const Arguments& args) {

    HandleScope scope;

    Local<String> path = args[0]->ToString();
    Local<Function> callback = Local<Function>::Cast(args[1]);

    init_params* request = new init_params;
    request->callback = Persistent<Function>::New(callback);

    String::AsciiValue argpath(path);

    request->path = (char *) malloc(argpath.length() + 1);
    strcpy(request->path,*argpath);

    uv_work_t* req = new uv_work_t();
    req->data = request;

    uv_queue_work(uv_default_loop(), req, init_repository_worker,(uv_after_work_cb)init_repository_callback);

    return Undefined();
}
