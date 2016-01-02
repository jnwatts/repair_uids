#pragma once

#include <string>
#include <list>
#include <map>

class IdEntry;
typedef unsigned int id_t;
typedef std::list<IdEntry*> IdEntryList;
typedef std::map<id_t, IdEntry*> Int2IdEntryMap;

#define INVALID_ID ((id_t)-1)

class IdDb
{
public:
    IdDb(void);
    ~IdDb(void);

    void read(std::string orig_passwd, std::string new_passwd, std::string orig_group, std::string new_group);

    uid_t transformUid(uid_t uid) { return transform(uid, this->uidIndex); }
    gid_t transformGid(gid_t gid) { return transform(gid, this->gidIndex); }

private:
    void readFile(std::string filename, IdEntryList &list, std::string list_name, bool is_orig);
    void buildIndex(IdEntryList &list, Int2IdEntryMap &index, std::string list_name);
    IdEntry &getEntry(std::string name, IdEntryList &list);
    id_t transform(id_t id, Int2IdEntryMap &index);

    IdEntryList uids, gids;

    Int2IdEntryMap uidIndex;
    Int2IdEntryMap gidIndex;
};
