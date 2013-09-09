#include "sgit.h"

using namespace v8;

void init_repository_worker(uv_work_t* req);
void init_repository_callback(uv_work_t* req);
extern "C" Handle<Value> init_repository(const Arguments& args);

