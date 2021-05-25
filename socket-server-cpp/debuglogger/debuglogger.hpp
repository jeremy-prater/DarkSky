#pragma once

#include <mutex>
#include <string>

#define DEBUG_LINE_LENGTH 1024

namespace net {
namespace pdev {

class DebugLogger {
public:
  typedef enum {
    ERROR,
    WARNING,
    STATUS,
    INFO,
    VERBOSE,
    NUM_LEVELS,
  } Level;

  typedef enum {
    RESET,
    RED,
    GREEN,
    BLUE,
    CYAN,
    MAGENTA,
    YELLOW,
    WHITE,
    NUM_COLORS,
  } Color;

  [[nodiscard]] static const std::string
  ConvertBoolToString(bool value) noexcept;

  DebugLogger(std::string debugTag, Color color, bool bold);

  void Error(const char *format, ...) const noexcept;
  void Warning(const char *format, ...) const noexcept;
  void Status(const char *format, ...) const noexcept;
  void Info(const char *format, ...) const noexcept;
  void Verbose(const char *format, ...) const noexcept;

  void WriteLog(Level level, const char *format, ...) const noexcept;
  void SetModuleName(std::string newName) noexcept;
  void SetColor(Color newColor) noexcept;
  void SetBold(bool bold) noexcept;

private:
  static const char *Levels[Level::NUM_LEVELS];
  static const char *Colors[Color::NUM_COLORS];
  static std::mutex loggerLock;

  void WriteLogInternal(Level level, const char *format,
                        va_list args) const noexcept;

  std::string debugTag;
  Color color;
  bool debugBold;
};

}
}
