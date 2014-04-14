#include <stdio.h>
#include <iostream>
#include <string>
#include "dirList.h"

std::vector<std::string> listPresets(void) {
    DIR                 *d;
    struct dirent       *dir;
    std::vector<std::string>      dirlist;
    int i=0;
    d = opendir("./presets");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            i++;
            dirlist.push_back(dir->d_name);
        }
        closedir(d);
    }
    return dirlist;
}
