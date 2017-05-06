#ifndef SERVER_LOGGER_HPP
#define SERVER_LOGGER_HPP

#include <atomic>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
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

inline void _concat(std::stringstream& ss) {}

template<typename Arg1>
inline void _concat(std::stringstream& ss, Arg1&& arg1) {
    ss << arg1;
}

template<typename Arg1, typename ...Args>
inline void _concat(std::stringstream& ss, Arg1&& arg1, Args&& ...args) {
    ss << arg1;
    _concat(ss, std::forward<Args>(args)...);
}

template<typename ...Args>
inline std::string concat(Args&& ...args) {
    std::stringstream ss;
    _concat(ss, std::forward<Args>(args)...);
    return ss.str();
}

void trace(Logger&, const std::string& msg);
void debug(Logger&, const std::string& msg);
void info(Logger&, const std::string& msg);
void warn(Logger&, const std::string& msg);
void error(Logger&, const std::string& msg);
#endif
