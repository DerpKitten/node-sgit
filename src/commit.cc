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

using namespace v8;

struct commit_params {
	char* path;
	char* file;
	char* message;
        char commit_id[GIT_OID_HEXSZ + 1];
	const char* err;
	Persistent<Function> callback;
};

void commit_worker(uv_work_t* req) {

	commit_params* request = (commit_params*)req->data;
	const char* err = "";

	git_repository *repo;
        git_signature *sig;
        git_index *index;
        git_oid tree_id, commit_id;
        git_tree *tree;

        if (git_repository_open(&repo, request->path) < 0) {
                err = "Opening repository failed, git_repository_open returned non zero value";
        }

        if (git_repository_index(&index, repo) < 0) {
                err = "Opening index failed, git_repository_index returned non zero value";
        }

        if (git_index_add_bypath(index, request->file) < 0) {
                err = "Index add failed, git_index_add_bypath returned non zero value";
        }
        git_index_write(index);

        git_commit *parent = NULL;

        // it is okay if looking up the HEAD fails
        git_revparse_single((git_object **)&parent, repo, "HEAD");

        git_index_write_tree(&tree_id, index);

        //refresh index
        git_repository_index(&index, repo);

        git_tree_lookup(&tree, repo, &tree_id);

	// use default repo signature info if present in repo config, otherwise use sgite default

        if (git_signature_default(&sig, repo) < 0) {
                git_signature_now(&sig, "Node-sgit https://github.com/derpkitten/node-sgit", "none@example.com");
        }

        git_commit_create_v(&commit_id, repo, "HEAD", sig, sig, NULL, request->message, tree, parent ? 1 : 0, parent);

        git_oid_tostr(request->commit_id,sizeof(request->commit_id),&commit_id);

	request->err = err;

        //free allocated git resources
        git_tree_free(tree);
        git_signature_free(sig);
        git_index_free(index);
        git_repository_free(repo);
}

void commit_callback(uv_work_t* req) {
	HandleScope scope;

	commit_params* request = (commit_params*)req->data;
	delete req;

	Handle<Value> argv[2] = { Local<Value>::New(String::New(request->err)),Local<Value>::New(String::New(request->commit_id)) };

	request->callback->Call(Context::GetCurrent()->Global(), 2, argv);

    	request->callback.Dispose();

    	delete request;

}

extern "C" Handle<Value> commit_bypath(const Arguments& args) {

    HandleScope scope;

    Local<String> path = args[0]->ToString();
    Local<String> file = args[1]->ToString();
    Local<String> message = args[2]->ToString();
    Local<Function> callback = Local<Function>::Cast(args[3]);

    commit_params* request = new commit_params;
    request->callback = Persistent<Function>::New(callback);

    String::AsciiValue argpath(path);
    String::AsciiValue argfile(file);
    String::AsciiValue argmessage(message);

    request->path = (char *) malloc(argpath.length() + 1);
    strcpy(request->path,*argpath);

    request->file = (char *) malloc(argfile.length() + 1);
    strcpy(request->file,*argfile);

    request->message = (char *) malloc(argmessage.length() + 1);
    strcpy(request->message,*argmessage);

    uv_work_t* req = new uv_work_t();
    req->data = request;

    uv_queue_work(uv_default_loop(), req, commit_worker,(uv_after_work_cb)commit_callback);

    return Undefined();
}

