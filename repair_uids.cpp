#include <assert.h>
#include <getopt.h>
#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "iddb.h"
#include "dig.h"
#include "string_utils.h"

using namespace std;

string orig_passwd = "passwd.orig";
string new_passwd = "passwd.new";
string orig_group = "group.orig";
string new_group = "group.new";
string log_path = getenv("PWD");
bool dry_run = false;
list<string> paths;

void usage(int argc, char *argv[])
{
    //printf("Usage: %s <orig passwd> <orig group> <new passwd> <new group> [OKGO] [path [path [path ...]]]\n", argv[0]);
    fprintf(stderr, "Usage: %s <options> [path [path [path ...]]]\n", argv[0]);
    fprintf(stderr, "\n"
            "  -p, --passwd               Passwd files (default: passwd.orig:passwd.new)\n"
            "  -g, --group                Group files (default: group.orig:group.new)\n"
            "  -l, --log                  Path to write write log files, - to use stdout (default: ${PWD})\n"
            "  --dry-run                  Don't actually modify files (still writes log files)\n"
            );
}

void parseFilePair(string const &arg, string &orig_file, string &new_file)
{
    vector<string> files = split_string(':', arg);
    assert(files.size() == 2);
    orig_file = files[0];
    new_file = files[1];
}

void parseOpts(int argc, char *argv[])
{
    enum args {
        PASSWD = 'p',
        GROUP = 'g',
        LOG = 'l',
        DRY_RUN = 0x100,
    };
    static const char short_options[] = "p:g:l:";
    static struct option long_options[] = {
        {"passwd",  required_argument, 0, PASSWD},
        {"group",   required_argument, 0, GROUP},
        {"log",     required_argument, 0, LOG},
        {"dry-run", no_argument,       0, DRY_RUN},
        {0,         0,                 0,  0 }
    };
    int c, opt_index = 0;

    paths.clear();
    do {
        c = getopt_long(argc, argv, short_options, long_options, &opt_index);
        switch (c) {
            case PASSWD:
                parseFilePair(optarg, orig_passwd, new_passwd);
                break;
            case GROUP:
                parseFilePair(optarg, orig_group, new_group);
                break;
            case LOG:
                log_path = optarg;
                break;
            case DRY_RUN:
                dry_run = true;
                break;
            default:
                break;
        }
    } while(c != -1);

    for (int i = optind; i < argc; i++) {
        paths.push_back(argv[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        usage(argc, argv);
        exit(0);
    }

    parseOpts(argc, argv);

#if 0
    printf("%s:%s, %s:%s, log:%s, dry_run:%d\n",
            orig_passwd.c_str(),
            new_passwd.c_str(),
            orig_group.c_str(),
            new_group.c_str(),
            log_path.c_str(),
            dry_run ? 1 : 0);

    for (string &path: paths) {
        printf(" %s\n", path.c_str());
    }
#endif

    IdDb db;
    db.read(orig_passwd, new_passwd, orig_group, new_group);

    for (string &path : paths) {
        string log_file;
        if (log_path == "-") {
            log_file = "-";
        } else {
            log_file = "log_" + path + ".txt";
            find_and_replace(log_file, "/", "_");
            log_file.insert(0, log_path + "/");
        }
        //Dig dig(path, &db, log_file, dry_run);
        //dig.repair();
    }

    return 0;
}
