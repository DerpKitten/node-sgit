/*
 * This is a sample program that is similar to "git init".  See the
 * documentation for that (try "git help init") to understand what this
 * program is emulating.
 *
 * This demonstrates using the libgit2 APIs to initialize a new repository.
 *
 * This also contains a special additional option that regular "git init"
 * does not support which is "--initial-commit" to make a first empty commit.
 * That is demonstrated in the "create_initial_commit" helper function.
 *
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include <stdio.h>
#include <git2.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* not actually good error handling */
static void fail(const char *msg, const char *arg)
{
	if (arg)
		fprintf(stderr, "%s %s\n", msg, arg);
	else
		fprintf(stderr, "%s\n", msg);
	exit(1);
}

/* forward declaration of helper to make an empty parent-less commit */
static void create_initial_commit(git_repository *repo);

int main()
{
	git_repository *repo = NULL;
	const git_oid *oid;
	char oidstr[GIT_OID_HEXSZ + 1];
	git_threads_init();

		if (git_repository_open(&repo, "./sandbox") < 0)
			fail("Could not initialize repository", " ");

	git_revspec revspec;

	git_revparse(&revspec,repo,"HEAD");	

	oid = git_object_id(revspec.from);

	git_oid_tostr(oidstr,sizeof(oidstr),oid);

	printf("oid: %s\n",oidstr);

	git_revwalk *walk;
	git_commit *wcommit;

  git_revwalk_new(&walk, repo);
  git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL | GIT_SORT_REVERSE);
  git_revwalk_push(walk, git_object_id(revspec.from));

  const git_signature *cauth;
  const char *cmsg;

	git_oid *walkoid =  (git_oid *)oid;
	int error;
	while ((git_revwalk_next(walkoid, walk)) == 0) {
    		error = git_commit_lookup(&wcommit, repo, walkoid);
        	//check_error(error, "looking up commit during revwalk");

    cmsg  = git_commit_message(wcommit);
    cauth = git_commit_author(wcommit);
    printf("%s (%s)\n", cmsg, cauth->email);

    git_commit_free(wcommit);
  }

  git_revwalk_free(walk);
	return 0;








	/* As an extension to the basic "git init" command, this example
	 * gives the option to create an empty initial commit.  This is
	 * mostly to demonstrate what it takes to do that, but also some
	 * people like to have that empty base commit in their repo.
	 */
		create_initial_commit(repo);
		printf("Created empty initial commit\n");

	git_repository_free(repo);
	git_threads_shutdown();

	return 0;
}

/* Unlike regular "git init", this example shows how to create an initial
 * empty commit in the repository.  This is the helper function that does
 * that.
 */
static void create_initial_commit(git_repository *repo)
{
	git_signature *sig;
	git_index *index;
	git_oid tree_id, commit_id;
	git_tree *tree;

	/* First use the config to initialize a commit signature for the user */

	/* Now let's create an empty tree for this commit */

	if (git_repository_index(&index, repo) < 0)
		fail("Could not open repository index", NULL);

	/* Outside of this example, you could call git_index_add_bypath()
	 * here to put actual files into the index.  For our purposes, we'll
	 * leave it empty for now.
	 */
	if (git_index_add_bypath(index,"test.c") < 0)
		fail("Could not add file to index", NULL);
		git_index_write(index);
	//if (git_index_write_tree(&tree_id, index) < 0)
//		fail("Unable to write initial tree from index", NULL);


        git_commit *parent = NULL;

        git_revparse_single((git_object **)&parent, repo, "HEAD");
        /* it is okay if looking up the HEAD fails */

        git_index_write_tree(&tree_id, index);
        //git_index_write(index); /* not needed, but might as well */

        git_repository_index(&index, repo);
        git_tree_lookup(&tree, repo, &tree_id);

        git_signature_now(&sig, "Testy McTester", "tt@tester.test");

        git_commit_create_v(
                &commit_id, repo, "HEAD", sig, sig,
                NULL, "first commit", tree, parent ? 1 : 0, parent);


	git_tree_free(tree);
	git_signature_free(sig);
}
