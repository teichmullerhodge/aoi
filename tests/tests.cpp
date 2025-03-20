#include "../src/aoi/aoi.hpp"
#include "../src/declarations/declarations.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>

static const str LOCAL_URL = "http://localhost:5000/items";
static const str EXTERN_URL = "https://jsonplaceholder.typicode.com/posts";

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
      aoi::perform(LOCAL_URL, {AOINET::_GET, DEFAULT_HEADERS, "", false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_GET);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_URL, {AOINET::_GET, DEFAULT_HEADERS, "", false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_GET);
      });

  /// SSL | Blocking
  auto SSLBlocking =
      aoi::perform(EXTERN_URL, {AOINET::_GET, DEFAULT_HEADERS, "", true});

  assert_status(SSLBlocking.get_status(), AOINET::_GET);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_URL, {AOINET::_GET, DEFAULT_HEADERS, "", true},
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
      LOCAL_URL, {AOINET::_POST, DEFAULT_HEADERS, localPayload, false});

  assert_status(nonSSLBlocking.get_status(), AOINET::_POST);
  /// NON SSL | Async
  aoi::async_perform(
      LOCAL_URL, {AOINET::_POST, DEFAULT_HEADERS, localPayload, false},
      then(aoihttp nonSSLAsync) {
        assert_status(nonSSLAsync.get_status(), AOINET::_POST);
      });

  /// SSL | Blocking
  auto SSLBlocking = aoi::perform(
      EXTERN_URL, {AOINET::_POST, DEFAULT_HEADERS, externPayload, true});

  assert_status(SSLBlocking.get_status(), AOINET::_POST);
  /// SSL | Async
  aoi::async_perform(
      EXTERN_URL, {AOINET::_POST, DEFAULT_HEADERS, externPayload, true},
      then(aoihttp SSLAsync) {
        assert_status(SSLAsync.get_status(), AOINET::_POST);
      });
  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(loop);
}
void patch() {}
void put() {}
void _delete() {}

s32 main(void) {
  get();
  post();
}
