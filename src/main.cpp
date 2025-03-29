#include "aoi/aoi.hpp"
#include "aoi/aoimotion.hpp"
#include "declarations/declarations.hpp"
#include <iostream>
#include <uv.h>

#define SAMPLE_URL "https://jsonplaceholder.typicode.com/users"

s32 main(int argc, char **argv) {

  (void)argc;
  (void)argv;
  setenv("UV_THREADPOOL_SIZE", "70", 1);

  motion *loop = uv_default_loop();

  /// async call example
  aoi::async_perform(
      SAMPLE_URL, DEFAULT_BUILDER,
      then(aoihttp h) { std::cout << h.responseStream << "\n"; });

  /// sync call example
  auto response = aoi::perform(SAMPLE_URL);

  std::cout << response.responseStream << "\n";
  std::cout << response.get_status() << "\n";

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
  return 0;
}