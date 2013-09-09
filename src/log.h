#include "sgit.h"

using namespace v8;

void log_repository_worker(uv_work_t* req);
void log_repository_callback(uv_work_t* req);
extern "C" Handle<Value> log_repository(const Arguments& args);

