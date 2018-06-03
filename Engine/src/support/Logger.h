#ifndef ENGINE_LOGGER_H
#define ENGINE_LOGGER_H

#include <string>

/**
 * Persist or output log messages
 */
class Logger {
 public:
  static void info(std::string message);
  static void warn(std::string message);
  static void error(std::string message);
};

#endif //ENGINE_LOGGER_H
