#include "Logger.hpp"
#include <thread>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

std::ostream &operator<<(std::ostream &ostr, Level level) {
  switch (level) {
  case Level::TRACE:
    ostr << "TRACE";
    break;
  case Level::DEBUG:
    ostr << "DEBUG";
    break;
  case Level::INFO:
    ostr << "INFO";
    break;
  case Level::WARN:
    ostr << "WARN";
    break;
  case Level::ERROR:
    ostr << "ERROR";
    break;
  default:
    ostr << "n/a";
    break;
  }
  return ostr;
}

namespace bpt = boost::posix_time;
using second_clock = boost::posix_time::second_clock;

Logger::Logger(const std::string &p)
    : path(p), levelVal(Level::INFO), isWriting(false) {
  out.open(path, std::ofstream::out | std::ofstream::app);
}

Logger::~Logger() { out.close(); }

// Synchronically (using spin-lock like and atomic instead of mutexes)
// writes the message to the log and flush it.
void Logger::syncWrite(Level level, const std::string &msg) {
  if (level >= levelVal) {
    auto flagExpected = false;
    while (!isWriting.compare_exchange_weak(flagExpected, true)) {

      flagExpected = false;
      std::this_thread::yield();
    }
    out << second_clock::local_time() << "\t" << level << "\t" << msg
        << std::endl;
    isWriting.store(false);
  }
}

void trace(Logger &logger, const std::string &msg) {
  if (logger.level() <= Level::TRACE) {
    logger.syncWrite(Level::TRACE, msg);
  }
}

void debug(Logger &logger, const std::string &msg) {
  if (logger.level() <= Level::DEBUG) {
    logger.syncWrite(Level::DEBUG, msg);
  }
}
void info(Logger &logger, const std::string &msg) {
  if (logger.level() <= Level::INFO) {
    logger.syncWrite(Level::INFO, msg);
  }
}
void warn(Logger &logger, const std::string &msg) {
  if (logger.level() <= Level::WARN) {
    logger.syncWrite(Level::WARN, msg);
  }
}
void error(Logger &logger, const std::string &msg) {
  if (logger.level() <= Level::ERROR) {
    logger.syncWrite(Level::ERROR, msg);
  }
}
