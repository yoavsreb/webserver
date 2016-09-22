#ifndef SERVER_LOGGER_HPP
#define SERVER_LOGGER_HPP

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <boost/lexical_cast.hpp>

enum Level {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};

std::ostream& operator<<(std::ostream& ostr, Level level) {
    switch(level) {
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

class Logger {
public:
    
    Logger(const std::string& p) : path(p), levelVal(Level::INFO), isWriting(false) {
        out.open(path, std::ofstream::out | std::ofstream::app); 
    }

    ~Logger() {
        out.close();
    }

    std::ostream& stream() {
        return out;
    }
      
    Level level() const {
        return levelVal;
    }

    void syncWrite(Level level, const std::string& msg) {
        if (level >= levelVal) {
            auto flagExpected = false;
            while(!isWriting.compare_exchange_strong(flagExpected, true)) {
                std::this_thread::yield();
            }
            out << level << "\t" << msg << std::endl;
            isWriting.store(false);
        }
    }

private:
    const std::string path;
    const Level levelVal;
    std::ofstream out;    
    std::atomic_bool isWriting;
};

void log(Logger& logger, const std::string& msg) {
    if (logger.level() >= Level::INFO) {
        logger.syncWrite(Level::INFO, msg);
    }
}

#endif
