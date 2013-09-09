#include "sgit.h"

using namespace v8;

void commit_worker(uv_work_t* req);
void commit_callback(uv_work_t* req);
extern "C" Handle<Value> commit_bypath(const Arguments& args);

