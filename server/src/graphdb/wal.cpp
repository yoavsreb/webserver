#include "graphdb/wal.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

namespace bf = boost::filesystem;

namespace server {
bool writeWALMessage(int fd, const WAL_Message* message) {
    write(fd, message, WAL_Message::TOTAL_SIZE);
    return fsync(fd) == 0;
}  

std::vector<WAL_Message> readNonCommitted(const std::string& filename) {
    std::vector<WAL_Message> retval;
    std::ifstream backupFileStream(filename, std::ios::binary | std::ios::in);
    while(backupFileStream) {
        WAL_Message message;
        if (backupFileStream.read(message.toCharPtr(), WAL_Message::TOTAL_SIZE)) {
            if (message.type == 0) {
                retval.clear();    
            } else {
                retval.push_back(message);
            }
        } else {
            break;
        }
    }
    return retval; 
}

WAL::~WAL() {
    if (isReady()) {
        close(fd);
    }
}

std::vector<WAL_Message> WAL::init() {
    std::lock_guard<std::mutex> guard(mutex);
    if (bf::exists(filename)) {
        // 1. Create copy
        // 2. Scan it
        // 3. Set WAL and last commited location, 
        //  returning all non-committed lines.
        std::string backup{filename + ".bck"};
        bf::rename(filename, backup);
        std::vector<WAL_Message> nonCommitted = readNonCommitted(backup);
        
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        auto bytesToWrite = nonCommitted.size() * WAL_Message::TOTAL_SIZE;
        auto end = lseek(fd, -1 * bytesToWrite, SEEK_END);
        ftruncate(fd, end);
        fsync(fd);
        return nonCommitted;
    } else {
        // New file. We just need to open it
        // and return an empty collection
        fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        return std::vector<WAL_Message>{};
    }
}

void WAL::writeSnapshotMarker() {
    std::lock_guard<std::mutex> guard(mutex);
    WAL_Message message;
    std::memset(&message, 0, WAL_Message::TOTAL_SIZE);
    static std::string snapshotMessage("SNAPSHOT"); 
    std::memcpy(message.message, snapshotMessage.c_str(), snapshotMessage.size()); 
    message.messageLength = snapshotMessage.size();
    writeWALMessage(fd, &message);
}

bool WAL::write(const WAL_Message* message) { 
    if (isReady()) {
        std::lock_guard<std::mutex> guard(mutex);
        return writeWALMessage(fd, message);
    } else {
        return false;
    }
}

}; // namespace
