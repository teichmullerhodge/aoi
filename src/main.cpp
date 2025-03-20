#include "aoi/aoi.hpp"
#define LOCAL_URL "http://localhost:5000/items"
#define SAMPLE_URL "https://jsonplaceholder.typicode.com/posts"

int main(int argc, char **argv){ unused

    setenv("UV_THREADPOOL_SIZE", "30", 1); // or use your local env.

    aoibuilder b = {
        AOINET::_GET,
        DEFAULT_HEADERS,
        "",
        true

    };

    std::vector<str> urls(100, SAMPLE_URL);
    std::vector<aoibuilder> builders(100, b);    
    aoi::perform_as_batch(urls, builders);
    return 0;

}