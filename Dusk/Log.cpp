export module Dusk.Log;

import std;

namespace Dusk {

export enum class LogLevel : std::uint8_t {
  Debug,
  Info,
  Warn,
  Error,
  Critical
};

export auto log(Dusk::LogLevel level, std::string_view tag,
                std::string_view message) -> void {
  std::string printableLevel = "";
  switch (level) {
    case LogLevel::Debug:
      printableLevel = "[DEBUG]";
      break;
    case LogLevel::Info:
      printableLevel = "[INFO ]";
      break;
    case LogLevel::Warn:
      printableLevel = "[WARN ]";
    case LogLevel::Error:
      printableLevel = "[ERROR]";
    case LogLevel::Critical:
      printableLevel = "[CRIT ]";
      break;
  }

  const auto now = std::chrono::system_clock::now();
  const auto sec = std::chrono::floor<std::chrono::seconds>(now);
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - sec) % 1000;
  const auto timestamp = std::format("{:%H:%M:%S}.{:03}", sec, ms.count());

  std::print("{} {} {} {}", timestamp, printableLevel, tag, message);
}

export auto log(Dusk::LogLevel level, std::string_view message) -> void {
  std::string printableLevel = "";
  switch (level) {
    case LogLevel::Debug:
      printableLevel = "[DEBUG]";
      break;
    case LogLevel::Info:
      printableLevel = "[INFO ]";
      break;
    case LogLevel::Warn:
      printableLevel = "[WARN ]";
    case LogLevel::Error:
      printableLevel = "[ERROR]";
    case LogLevel::Critical:
      printableLevel = "[CRIT ]";
      break;
  }

  const auto now = std::chrono::system_clock::now();
  const auto sec = std::chrono::floor<std::chrono::seconds>(now);
  const auto ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - sec) % 1000;
  const auto timestamp = std::format("{:%H:%M:%S}.{:03}", sec, ms.count());

  std::print("{} {} {}", timestamp, printableLevel, message);
}

}  // namespace Dusk
