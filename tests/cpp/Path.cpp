/*
 * Path.cpp
 *
 *  Created on: Jul 5, 2021
 *      Author: mballance
 */

#include "Path.h"
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <spawn.h>
#include <unistd.h>
#include <fcntl.h>
#include "Env.h"


Path::Path() {
	// TODO Auto-generated constructor stub

}

Path::~Path() {
	// TODO Auto-generated destructor stub
}

bool Path::isdir(const std::string &path) {
        struct stat sb;
        return (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
}

bool Path::isfile(const std::string &path) {
        struct stat sb;
        return (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode));
}

std::string Path::abspath(const std::string &path) {
        char tmp[1024];
        realpath(path.c_str(), tmp);
        return tmp;
}

std::string Path::findpath(const std::string &path) {
        std::string fullpath = abspath(path);
        if (isfile(fullpath)) {
                return fullpath;
        } else {
                return "";
        }
}

std::string Path::join(const std::string &root, const std::string &leaf) {
        return root + "/" + leaf;
}

std::string Path::join(std::initializer_list<std::string> elems) {
        std::string ret;

        uint32_t i=0;
        for (std::initializer_list<std::string>::iterator it=elems.begin();
                        it!=elems.end(); it++) {
                if (i) {
                        ret += "/";
                }
                ret += *it;
                i++;
        }

        return ret;
}

std::string Path::dirname(const std::string &path) {
        int32_t idx;
        if ((idx=path.rfind('/')) != std::string::npos) {
                return path.substr(0, idx);
        } else {
                return "";
        }
}

std::string Path::basename(const std::string &path) {
        int32_t idx;
        if ((idx=path.rfind('/')) != std::string::npos) {
                return path.substr(idx+1);
        } else {
                return path;
        }
}

bool Path::mkdir(const std::string &path) {
        std::string tmp = path;
        if (tmp.at(tmp.size()-1) == '/') {
                tmp = tmp.substr(0, tmp.size()-1);
        }
        int32_t idx=1;

        while ((idx=tmp.find('/', idx)) != std::string::npos) {
                std::string elem = tmp.substr(0, idx);
                ::mkdir(elem.c_str(), S_IRWXU);
                idx++;
        }

        ::mkdir(path.c_str(), S_IRWXU);

        return true;
}

bool Path::rmdir(const std::string &path) {
    size_t path_len;
    char *full_path;
    DIR *dir;
    struct stat stat_path, stat_entry;
    struct dirent *entry;

    // stat for the path
    stat(path.c_str(), &stat_path);

    // if path does not exists or is not dir - exit with status -1
    if (S_ISDIR(stat_path.st_mode) == 0) {
        // Doesn't exist
        return false;
    }

    // if not possible to read the directory for this user
    if ((dir = opendir(path.c_str())) == NULL) {
        return false;
    }

    // the length of the path
    path_len = strlen(path.c_str());

    // iterate through directories
    while ((entry = readdir(dir)) != NULL) {

        // skip entries "." and ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        // determinate a full path of an entry
        std::string filename = path + "/" + entry->d_name;

        // recursively remove a nested directory
        if (isdir(filename)) {
            rmdir(filename);
            continue;
        }

        // remove a file object
        if (unlink(filename.c_str()) != 0) {
            fprintf(stdout, "Error: Can't remove file: %s\n", filename.c_str());
        }
    }

    // Remove the empty directory
    if (::rmdir(path.c_str()) != 0) {
        fprintf(stdout, "Error: Can't remove directory: %s\n", path.c_str());
    }

    closedir(dir);

    return true;
}

