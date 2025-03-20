#include "aoi/aoi.hpp"
#include "aoi/aoimotion.hpp"
#include "declarations/declarations.hpp"
#include "motion/motion_engine.hpp"
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <uv.h>

#define SAMPLE_URL "https://jsonplaceholder.typicode.com/users"

s32 main(int argc, char **argv) {

  (void)argc;
  (void)argv;

  aoibuilder b = DEFAULT_BUILDER;

  motion *loop = uv_default_loop();
  aoi::async_perform(SAMPLE_URL, DEFAULT_BUILDER, [](aoihttp result) {
    std::cout << result.responseStream << "\n";
  });
  aoi::async_perform(
      SAMPLE_URL, DEFAULT_BUILDER,
      then(aoihttp r) { std::cout << r.responseStream << "\n"; });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
  return 0;
}