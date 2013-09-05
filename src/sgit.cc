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

using namespace std;

inline string replaceAll(const string& s, const string& f, const string& r) {
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


Handle<Value> init_repository(const Arguments& args) {
	
	HandleScope scope;

	Local<String> path = args[0]->ToString();
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

	return scope.Close(Undefined());
}


Handle<Value> commit_bypath(const Arguments& args) {

	HandleScope scope;

	Local<String> path = args[0]->ToString();
	Local<String> file = args[1]->ToString();
	Local<String> message = args[2]->ToString();
	Local<Function> callback = Local<Function>::Cast(args[3]);
	const char *err = "";

	//AsciiValue flavor overides * operator to return const char
	String::AsciiValue argpath(path);
	String::AsciiValue argfile(file);
	String::AsciiValue argmessage(message);

	git_repository *repo;
	git_signature *sig;
        git_index *index;
        git_oid tree_id, commit_id;
        git_tree *tree;

        if (git_repository_open(&repo, *argpath) < 0) {
		err = "Opening repository failed, git_repository_open returned non zero value";
	}

        if (git_repository_index(&index, repo) < 0) {
		err = "Opening index failed, git_repository_index returned non zero value";
	}

        if (git_index_add_bypath(index, *argfile) < 0) {
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

	if (git_signature_default(&sig, repo) < 0) {
		git_signature_now(&sig, "Node-sgit https://github.com/lushzero/node-sgit", "none@example.com");
	}

        git_commit_create_v(&commit_id, repo, "HEAD", sig, sig, NULL, *argmessage, tree, parent ? 1 : 0, parent);

	//free allocated git resources
        git_tree_free(tree);
        git_signature_free(sig);
        git_index_free(index);
        git_repository_free(repo);

	char completed_commit_id[GIT_OID_HEXSZ + 1];
	git_oid_tostr(completed_commit_id,sizeof(completed_commit_id),&commit_id);

	Local<Value> argv[2] = { Local<Value>::New(String::New(err)),Local<Value>::New(String::New(completed_commit_id)) };
	callback->Call(Context::GetCurrent()->Global(), 2, argv);

	return scope.Close(Undefined());
}




Handle<Value> log(const Arguments& args) {

        HandleScope scope;

        Local<String> path = args[0]->ToString();
        Local<Function> callback = Local<Function>::Cast(args[1]);
        const char *err = "";

        //AsciiValue flavor overides * operator to return const char
        String::AsciiValue argpath(path);

        git_repository *repo;
        const git_oid *oid;

        if (git_repository_open(&repo, *argpath) < 0) {
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

        Local<Value> argv[2] = { Local<Value>::New(String::New(err)), Local<String>::New(String::New(stmp.c_str())) };
        callback->Call(Context::GetCurrent()->Global(), 2, argv);

        return scope.Close(Undefined());
}


std::string str_replace(const std::string& search, const std::string& replace, const std::string& subject)
{
    std::string str = subject;
    size_t pos = 0;
    while((pos = str.find(search, pos)) != std::string::npos)
    {
        str.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return str;
}



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

	exports->Set(String::NewSymbol("log"),FunctionTemplate::New(log)->GetFunction());

	//alternative form to export as JS constructor rather than individual functions, not needed ATM, kept for reference
	//module->Set(String::NewSymbol("exports"),FunctionTemplate::New(Method)->GetFunction());
}

NODE_MODULE(sgit, init)
