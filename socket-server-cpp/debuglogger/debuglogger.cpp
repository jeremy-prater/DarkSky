#include "debuglogger/debuglogger.hpp"
#include <iostream>
#include <stdarg.h>
#include <stdio.h>

namespace net {
namespace pdev {

[[nodiscard]] const std::string
DebugLogger::ConvertBoolToString(bool value) noexcept {
  return value ? "true" : "false";
}

const char *DebugLogger::Levels[DebugLogger::Level::NUM_LEVELS] = {
    "     ERROR", "   WARNING", "  STATUS  ", "  INFO    ", "VERBOSE   ",
};

const char *DebugLogger::Colors[DebugLogger::Color::NUM_COLORS] = {
    "\x1B[0m",     "\x1B[%d;31m", "\x1B[%d;32m", "\x1B[%d;34m",
    "\x1B[%d;36m", "\x1B[%d;35m", "\x1B[%d;33m", "\x1B[%d;37m"};

std::mutex DebugLogger::loggerLock;

DebugLogger::DebugLogger(std::string debugTag, Color color, bool bold)
    : debugTag(debugTag), color(color), debugBold(bold) {}

void DebugLogger::SetModuleName(std::string newName) noexcept {
  debugTag = newName;
}

void DebugLogger::SetColor(Color newColor) noexcept { color = newColor; }

void DebugLogger::SetBold(bool bold) noexcept { debugBold = bold; }

void DebugLogger::Error(const char *format, ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(DebugLogger::Level::ERROR, format, args);
  va_end(args);
}

void DebugLogger::Warning(const char *format, ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(DebugLogger::Level::WARNING, format, args);
  va_end(args);
}

void DebugLogger::Status(const char *format, ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(DebugLogger::Level::STATUS, format, args);
  va_end(args);
}

void DebugLogger::Info(const char *format, ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(DebugLogger::Level::INFO, format, args);
  va_end(args);
}

void DebugLogger::Verbose(const char *format, ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(DebugLogger::Level::VERBOSE, format, args);
  va_end(args);
}

void DebugLogger::WriteLog(DebugLogger::Level level, const char *format,
                           ...) const noexcept {
  va_list args{};
  va_start(args, format);
  WriteLogInternal(level, format, args);
  va_end(args);
}

void DebugLogger::WriteLogInternal(DebugLogger::Level level, const char *format,
                                   va_list args) const noexcept {
  static char buffer[DEBUG_LINE_LENGTH];
  static char levelColor[8];
  static char tagColor[8];

  vsnprintf(buffer, DEBUG_LINE_LENGTH, format, args);

  int levelColorIndex;
  bool levelBold = false;

  switch (level) {
  case Level::ERROR:
    levelColorIndex = Color::RED;
    levelBold = true;
    break;
  case Level::WARNING:
    levelColorIndex = Color::YELLOW;
    levelBold = true;
    break;
  case Level::STATUS:
    levelColorIndex = Color::GREEN;
    break;
  case Level::INFO:
    levelColorIndex = Color::WHITE;
    break;
  case Level::VERBOSE:
    levelColorIndex = Color::CYAN;
    break;

  default:
    levelColorIndex = Color::MAGENTA;
    break;
  };

  snprintf(levelColor, 8, Colors[levelColorIndex], levelBold);
  snprintf(tagColor, 8, Colors[color], debugBold);

  {
    std::lock_guard<std::mutex> guard(loggerLock);
    std::cout << "[" << levelColor << Levels[level] << Colors[Color::RESET]
              << "] [" << tagColor << debugTag << Colors[Color::RESET] << "] "
              << buffer << std::endl;
  }
}

}
}