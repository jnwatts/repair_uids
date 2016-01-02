#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include "iddb.h"
#include "string_utils.h"

using namespace std;

class IdEntry
{
public:
    IdEntry(std::string name) :
        name(name),
        orig_id(INVALID_ID),
        new_id(INVALID_ID)
    { }

    std::string toString(void) {
        stringstream ss;
        ss << "{"
            << this->name << ", "
            << this->orig_id << "->"
            << this->new_id
            << "}";
        return ss.str();
    }

    std::string name;
    id_t orig_id;
    id_t new_id;
};

IdDb::IdDb(void)
{
}

IdDb::~IdDb(void)
{
    for (auto entry : this->uids) {
        delete entry;
    }
    for (auto entry : this->gids) {
        delete entry;
    }
}


void IdDb::read(std::string orig_passwd, std::string new_passwd, std::string orig_group, std::string new_group)
{
    this->readFile(orig_passwd, this->uids, "uids", true);
    this->readFile(orig_group, this->gids, "gids", true);
    this->readFile(new_passwd, this->uids, "uids", false);
    this->readFile(new_group, this->gids, "gids", false);

    this->buildIndex();
}

void IdDb::readFile(std::string filename, IdEntryList &list, std::string list_name, bool is_orig)
{
    string line;
    ifstream f;
    f.open(filename);
    while (!f.eof()) {
        getline(f, line);
        vector<string> fields = split_string(':', line);
        // Both group and passwd place name and id in same field indecies
        string &name = fields[0];
        string &id_str = fields[2];

        IdEntry &entry = this->getEntry(name, list);
        id_t &id = (is_orig ? entry.orig_id : entry.new_id);
        stringstream(id_str) >> id;
    }
}

void IdDb::buildIndex(void)
{
}

IdEntry &IdDb::getEntry(std::string name, IdEntryList &list)
{
    auto iter = find_if(list.begin(), list.end(), [&name](IdEntry* e) -> bool { return e->name == name; });
    if (iter == list.end()) {
        IdEntry *entry = new IdEntry(name);
        list.push_back(entry);
        return *entry;
    } else {
        return *(*iter);
    }
}

id_t IdDb::transform(id_t id, Int2IdEntryMap &index)
{
    return INVALID_ID;
}
