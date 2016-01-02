#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "iddb.h"

class Dig
{
public:
    Dig(std::string path, IdDb *db, std::string logFile, bool dryRun = true);
    ~Dig(void);

    void repair(void);

private:
    void repair(const char *path);
    void printStats(bool force = false);
    void openLog(void);
    void closeLog(void);
    void log(std::string path, uid_t orig_uid, gid_t orig_gid, uid_t new_uid, gid_t new_gid);
    
    std::string path;
    IdDb *db;
    bool dryRun;
    struct stat base;
    std::ostream *logOutput;
    std::fstream logFile;

    unsigned long files;
    unsigned long directories;
    unsigned long ignored;
    unsigned long errors;
};
