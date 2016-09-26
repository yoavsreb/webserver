#ifndef SERVER_LOGGER_HPP
#define SERVER_LOGGER_HPP

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>

enum class Level {
    TRACE,
    DEBUG,
    INFO, 
    WARN, 
    ERROR
};

std::ostream& operator<<(std::ostream& ostr, Level level);

class Logger {
public:
    
    explicit Logger(const std::string& p);

    ~Logger();

    Level level() const { return levelVal; }
    
    friend void trace(Logger&, const std::string& msg);
    friend void debug(Logger&, const std::string& msg);
    friend void info(Logger&, const std::string& msg);
    friend void warn(Logger&, const std::string& msg);
    friend void error(Logger&, const std::string& msg);
private:

    void syncWrite(Level level, const std::string& msg);

private:
    const std::string path;
    const Level levelVal;
    std::ofstream out;    
    std::atomic_bool isWriting;
};

void trace(Logger&, const std::string& msg);
void debug(Logger&, const std::string& msg);
void info(Logger&, const std::string& msg);
void warn(Logger&, const std::string& msg);
void error(Logger&, const std::string& msg);
#endif
