#ifndef _WEBSERVER_WAL_HPP
#define _WEBSERVER_WAL_HPP

#include <algorithm>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

/**
 * WAL - Write Ahead Log. 
 * A super efficient IO log. 
 * All write should be flushed to disk ASAP to guarantee highest consistency.
 * All write operations are guarded by mutex.
 **/
namespace server {

/**
 * A data-object WAL message.
 */
struct WAL_Message {
    static const std::streamsize TOTAL_SIZE = 128;
    
    char* toCharPtr() const noexcept { return static_cast<char*>(static_cast<void*>(const_cast<WAL_Message*>(this))); } 
    std::uint8_t type;
    std::uint8_t stepId;
    std::uint8_t txId;
    std::uint8_t messageLength;
    char message[124];

};

/**
 * A helper function to create WAL messages
 */
inline WAL_Message buildMessage(std::uint8_t type,
        std::uint8_t stepId,
        std::uint8_t txId,
        const std::string& message) {
    
    WAL_Message retval;
    std::memset(&retval, 0, static_cast<size_t>(WAL_Message::TOTAL_SIZE));
    retval.type = type;
    retval.stepId = stepId;
    retval.txId = txId;
    retval.messageLength = message.size();
    message.copy(retval.message, message.size());
    return retval;
}

class WAL {
public:
    WAL(const std::string& f) : filename(f), fd(-1) {};
    ~WAL();

    bool isReady() const { return (fd > 0); }
    
    /**
     * Initialize the WAL. Roll back to previous snapshot marker, 
     * returning all possibly following messages.
     */
    std::vector<WAL_Message> init(); 
    
    /**
     * Write a snaphot-marker. A special message that marks we're not
     * in a middle of tranaction.
     */
    void writeSnapshotMarker();
    
    /**
     * Writes a message to the WAL.
     */
    bool write(const WAL_Message* message); 
private:
    std::string filename;
    int fd;
    std::mutex mutex;
};

}

#endif
