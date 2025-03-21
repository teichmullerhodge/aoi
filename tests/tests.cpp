#include "../src/aoi/aoi.hpp"
#include "../src/declarations/declarations.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>

static const str LOCAL_GET_URL = "http://localhost:5000/items";
static const str LOCAL_POST_URL = "http://localhost:5000/items";
static const str LOCAL_PATCH_URL = "http://localhost:5000/name";
static const str LOCAL_PUT_URL = "http://localhost:5000/tasks";
static const str LOCAL_DELETE_ITEM_URL = "http://localhost:5000/items/1";
static const str LOCAL_DELETE_TASK_URL = "http://localhost:5000/tasks/1";

static const str EXTERN_GET_URL = "https://jsonplaceholder.typicode.com/posts";
static const str EXTERN_POST_URL = "https://jsonplaceholder.typicode.com/posts";
static const str EXTERN_PATCH_URL =
    "https://jsonplaceholder.typicode.com/posts/1";
static const str EXTERN_PUT_URL =
    "https://jsonplaceholder.typicode.com/posts/1";
static const str EXTERN_DELETE_URL =
    "https://jsonplaceholder.typicode.com/posts/1";

class Logger {
public:
  enum class LogLevel { INFO, SUCCESS, WARNING, ERROR };

  static void log(LogLevel level, const std::string &message) {
    std::string timestamp = get_timestamp();
    std::string levelStr;
    std::string colorCode;

    switch (level) {
    case LogLevel::INFO:
      levelStr = "[INFO]    ";
      colorCode = "\033[34m"; // Blue
      break;
    case LogLevel::SUCCESS:
      levelStr = "[SUCCESS] ";
      colorCode = "\033[32m"; // Green
      break;
    case LogLevel::WARNING:
      levelStr = "[WARNING] ";
      colorCode = "\033[33m"; // Yellow
      break;
    case LogLevel::ERROR:
      levelStr = "[ERROR]   ";
      colorCode = "\033[31m"; // Red
      break;
    }

    std::cout << colorCode << timestamp << " " << levelStr << message
              << "\033[0m" << std::endl;
  }

  static void info(const std::string &message) { log(LogLevel::INFO, message); }

  static void success(const std::string &message) {
    log(LogLevel::SUCCESS, message);
  }

  static void warning(const std::string &message) {
    log(LogLevel::WARNING, message);
  }

  static void error(const std::string &message) {
    log(LogLevel::ERROR, message);
  }

private:
  static std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm_info = std::localtime(&now_time);
    std::ostringstream oss;
    oss << std::put_time(tm_info, "[%Y-%m-%d %H:%M:%S]");
    return oss.str();
  }
};

void assert_status(u16 status, str METHOD) {

  std::cout << "[" << METHOD << "] ";
  if (!aoi::status_ok(status)) {
    Logger::error("Error making the request. Test failed.");
    throw std::runtime_error("Received non 200 status code");
  }
  Logger::success("Request ok.");
}

void get() {

  motion *loop = uv_default_loop();

  /// NON SSL | Blocking
  auto nonSSLBlocking =
      aoi::perform(LOCAL_GET_URL, {AOINET::_GET, DEFAULT_HEADERS, "", false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_GET);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_GET_URL, {AOINET::_GET, DEFAULT_HEADERS, "", false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_GET);
      });

  /// SSL | Blocking
  auto SSLBlocking =
      aoi::perform(EXTERN_GET_URL, {AOINET::_GET, DEFAULT_HEADERS, "", true});

  assert_status(SSLBlocking.get_status(), AOINET::_GET);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_GET_URL, {AOINET::_GET, DEFAULT_HEADERS, "", true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_GET);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}

void post() {
  motion *loop = uv_default_loop();

  str localPayload = R"({"item": "My item"})";
  str externPayload = R"({"title": "foo", "body": "bar"})";
  /// NON SSL | Blocking
  auto nonSSLBlocking = aoi::perform(
      LOCAL_POST_URL, {AOINET::_POST, DEFAULT_HEADERS, localPayload, false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_POST);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_POST_URL, {AOINET::_POST, DEFAULT_HEADERS, localPayload, false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_POST);
      });

  /// SSL | Blocking
  auto SSLBlocking = aoi::perform(
      EXTERN_POST_URL, {AOINET::_POST, DEFAULT_HEADERS, externPayload, true});

  assert_status(SSLBlocking.get_status(), AOINET::_POST);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_POST_URL, {AOINET::_POST, DEFAULT_HEADERS, externPayload, true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_POST);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}
void patch() {

  motion *loop = uv_default_loop();

  str localPayload = R"({"Name": "Matheus"})";
  str externPayload = R"({"title": "foo_patched", "body": "bar"})";
  /// NON SSL | Blocking
  auto nonSSLBlocking = aoi::perform(
      LOCAL_PATCH_URL, {AOINET::_PATCH, DEFAULT_HEADERS, localPayload, false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_PATCH);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_PATCH_URL, {AOINET::_PATCH, DEFAULT_HEADERS, localPayload, false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_PATCH);
      });

  /// SSL | Blocking
  auto SSLBlocking = aoi::perform(
      EXTERN_PATCH_URL, {AOINET::_PATCH, DEFAULT_HEADERS, externPayload, true});

  assert_status(SSLBlocking.get_status(), AOINET::_PATCH);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_PATCH_URL, {AOINET::_PATCH, DEFAULT_HEADERS, externPayload, true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_PATCH);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}
void put() {

  motion *loop = uv_default_loop();

  str localPayload = R"({"task": "My task"})";
  str externPayload = R"({"title": "foo_put", "body": "bar"})";
  /// NON SSL | Blocking
  auto nonSSLBlocking = aoi::perform(
      LOCAL_PUT_URL, {AOINET::_PUT, DEFAULT_HEADERS, localPayload, false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_PUT);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_PUT_URL, {AOINET::_PUT, DEFAULT_HEADERS, localPayload, false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_PUT);
      });

  /// SSL | Blocking
  auto SSLBlocking = aoi::perform(
      EXTERN_PUT_URL, {AOINET::_PUT, DEFAULT_HEADERS, externPayload, true});

  assert_status(SSLBlocking.get_status(), AOINET::_PUT);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_PUT_URL, {AOINET::_PUT, DEFAULT_HEADERS, externPayload, true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_PUT);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}

// delete is a c++ keyword
void _delete() {

  motion *loop = uv_default_loop();
  /// NON SSL | Blocking
  auto nonSSLBlocking = aoi::perform(
      LOCAL_DELETE_ITEM_URL, {AOINET::_DELETE, DEFAULT_HEADERS, "", false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_DELETE);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_DELETE_TASK_URL, {AOINET::_DELETE, DEFAULT_HEADERS, "", false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_DELETE);
      });

  /// SSL | Blocking
  auto SSLBlocking = aoi::perform(EXTERN_DELETE_URL,
                                  {AOINET::_DELETE, DEFAULT_HEADERS, "", true});

  assert_status(SSLBlocking.get_status(), AOINET::_DELETE);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_DELETE_URL, {AOINET::_DELETE, DEFAULT_HEADERS, "", true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_DELETE);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}

s32 main(void) {
  std::cout << "[START] ";
  std::cout << "Initializing the tests.\n Performing all HTTP methods blocking "
               "and non-blocking, with SSL authentication and no SSL."
            << "\n";
  get();
  post();
  patch();
  put();
  _delete();
  std::cout << "[END] ";
  Logger::success("All tests passed successfully.");
}
