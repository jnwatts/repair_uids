#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "iddb.h"
#include "dig.h"
#include "string_utils.h"

using namespace std;

Dig::Dig(std::string path, IdDb *db, std::string logFile, bool dryRun) :
    path(path),
    db(db),
    dryRun(dryRun),
    files(0),
    directories(0),
    ignored(0),
    errors(0)
{
    if (logFile == "-") {
        this->logOutput = &cout;
    } else {
        this->logFile.open(logFile, ios_base::out | ios_base::app);
        this->logOutput = &this->logFile;
    }
}

Dig::~Dig(void)
{
    if (this->logFile.is_open()) {
        this->logFile.close();
    }
}

void Dig::repair(void)
{
    int result = 0;

    result = ::lstat(this->path.c_str(), &this->base);
    this->clearLine();
    if (result != 0) {
        fprintf(stderr, "Failed to stat %s: %s\n", this->path.c_str(), strerror(errno));
        this->errors++;
    } else {
        fprintf(stderr, "Searching %s\n", this->path.c_str());
        this->repair(path.c_str());
    }
    printStats(true);
    printf("\n");
}

void Dig::repair(const char *path)
{
    int result = 0;
    struct stat stat;
    uid_t uid;
    gid_t gid;

    result = ::lstat(path, &stat);
    if (result != 0) {
        this->clearLine();
        fprintf(stderr, "Failed to stat %s: %s\n", path, strerror(errno));
        this->errors++;
        goto ignore;
    }
    if (S_ISDIR(stat.st_mode)) {
        if (stat.st_dev == base.st_dev) {
            // Path is directory and on same device
            this->directories++;
            DIR *d = opendir(path);
            if (!d) {
                this->clearLine();
                fprintf(stderr, "Failed to open directory %s: %s\n", path, strerror(errno));
                this->errors++;
                goto ignore;
            }
            struct dirent *ent;
            // Enumerate directory contents
            while ((ent = readdir(d)) != NULL) {
                if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                    continue;
                int len = strlen(path) + 1/* '/' */ + strlen(ent->d_name) + 1/* '\0' */;
                char p[len];
                strncpy(p, path, len);
                strncat(p, "/", len);
                strncat(p, ent->d_name, len);
                this->repair(p);
            }
            closedir(d);
        }
    } else {
        // Path is a file
    }

    uid = db->transformUid(stat.st_uid);
    gid = db->transformGid(stat.st_gid);

    if (uid == stat.st_uid && gid == stat.st_gid) {
        // No change, ignore
        this->ignored++;
        goto ignore;
    }

    this->files++;

    this->log(path, stat.st_uid, stat.st_gid, uid, gid);
    
    if (!this->dryRun)
        result = lchown(path, uid, gid);
    if (result != 0) {
        this->clearLine();
        fprintf(stderr, "Failed to chown file %s: %s\n", path, strerror(errno));
        this->errors++;
    }

ignore:
    printStats();
}

void Dig::printStats(bool force)
{
    static int last = 0;
    timeb tb;
    ftime(&tb);
    int cur = tb.millitm + (tb.time & 0xfffff) * 1000;
    if (cur - last > 100 || force) {
        last = cur;
        this->clearLine();
        fprintf(stderr, "Files: %10lu Directories: %10lu Ignored: %10lu Errors: %10lu",
                this->files,
                this->directories,
                this->ignored,
                this->errors);
    }
}

void Dig::log(std::string path, uid_t orig_uid, gid_t orig_gid, uid_t new_uid, gid_t new_gid)
{
    *this->logOutput
        << path << ": "
        << orig_uid << ":" << orig_gid
        << " to "
        << new_uid << ":" << new_gid
        << "\n";
}

