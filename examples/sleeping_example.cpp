#include <uv.h>
#include <chrono>
#include <thread>
#include <iostream>

void engine_sleep() {
    static int calls = 0;
    std::cout << "Sleeping now...called " << calls << " times" << "\n";
    calls++;
    using namespace std::this_thread; 
    using namespace std::chrono; 

    sleep_for(seconds(1));

}

void call_engine(uv_work_t* req){
    engine_sleep();
}
void call_engine_callback(uv_work_t* req, int status){
    printf("Work done! Running in the main loop.\n");
    delete req;  
}

int main(int argc, char **argv){

    setenv("UV_THREADPOOL_SIZE", "10", 1);
    uv_loop_t *loop = uv_default_loop();
    for(unsigned int i = 0; i < 10; i++){
        uv_work_t *req = new uv_work_t();
        uv_queue_work(loop, req, call_engine, call_engine_callback);
    }


    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
    return 0;

}
