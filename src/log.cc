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
#include <sstream>

using namespace v8;

struct log_params {
	char* path;
	const char* err;
	char* log_data;
	Persistent<Function> callback;
};

void log_repository_worker(uv_work_t* req) {

	log_params* request = (log_params*)req->data;
	const char* err = "";

	git_repository *repo;
        const git_oid *oid;

        if (git_repository_open(&repo, "./sandbox") < 0) {
                err = "Opening repository failed, git_repository_open returned non zero value";
        }

        git_revspec revspec;
        git_revparse(&revspec,repo,"HEAD");
        oid = git_object_id(revspec.from);

        git_revwalk *walk;
        git_commit *wcommit;

        git_revwalk_new(&walk, repo);
        git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);
        git_revwalk_push(walk, git_object_id(revspec.from));

        char oidstr[GIT_OID_HEXSZ + 1];
        git_oid walkoid;
        git_oid_cpy(&walkoid,oid);

        std::stringstream json_log;

        //Initialize JSON return string
        json_log << "{";

        while ((git_revwalk_next(&walkoid, walk)) == 0) {
                const git_signature *cauth;
                const char *cmsg;

                git_commit_lookup(&wcommit, repo, &walkoid);

                cmsg  = git_commit_message(wcommit);
                std::string stmp(cmsg);
                stmp = replaceAll(stmp,"\\","\\\\");
                stmp = replaceAll(stmp, "\"","\\\"");
                stmp = replaceAll(stmp, "\t","\\t");

                cauth = git_commit_author(wcommit);

                git_oid_tostr(oidstr,sizeof(oidstr),git_commit_id(wcommit));

                std::string snametmp(cauth->name);
                snametmp = replaceAll(snametmp, "\\","\\\\");
                snametmp = replaceAll(snametmp, "\"","\\\"");
                snametmp = replaceAll(snametmp, "\t","\\t");

                json_log << "\"" << oidstr
                        << "\":{\"message\":\"" << stmp
                        << "\",\"name\":\"" << snametmp
                        << "\",\"email\":\"" << cauth->email
                        << "\",\"time\":\"" << git_commit_time(wcommit) << "\","
                        << "\"time_offset\":\"" << git_commit_time_offset(wcommit) << "\""
                        << "},";

                git_commit_free(wcommit);
        }
        json_log.seekp(json_log.str().length()-1);
        json_log << "}";

        git_revwalk_free(walk);
        //free allocated git resources
        git_repository_free(repo);

        //do escaping for JSON string
        std::string stmp(json_log.str());
        stmp = replaceAll(stmp, "\r","");
        stmp = replaceAll(stmp, "\n","\\n");

	request->err = err;
    	request->log_data = (char *) malloc(stmp.length() + 1);
        strcpy(request->log_data,stmp.c_str());
}

void log_repository_callback(uv_work_t* req) {
	HandleScope scope;

	log_params* request = (log_params*)req->data;
	delete req;

        Local<Value> argv[2] = { Local<Value>::New(String::New(request->err)), Local<String>::New(String::New(request->log_data)) };

	request->callback->Call(Context::GetCurrent()->Global(), 2, argv);

    	request->callback.Dispose();

    	delete request;

}

extern "C" Handle<Value> log_repository(const Arguments& args) {

    HandleScope scope;

    Local<String> path = args[0]->ToString();
    Local<Function> callback = Local<Function>::Cast(args[1]);

    log_params* request = new log_params;
    request->callback = Persistent<Function>::New(callback);

    String::AsciiValue argpath(path);

   // request->path = (char *) malloc(argpath.length() + 1);
   // strcpy(request->path,*argpath);

    uv_work_t* req = new uv_work_t();
    req->data = request;

    uv_queue_work(uv_default_loop(), req, log_repository_worker,(uv_after_work_cb)log_repository_callback);

    return Undefined();
}
