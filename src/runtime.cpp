//
// Created by travor on 20/08/2023.
//

#include "runtime.h"

std::string bond::get_exe_path() {
    std::string path;
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    path = buffer;
#elif __linux__
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (len != -1) {
        buffer[len] = '\0';
        path = buffer;
    }
#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        path = buffer;
    }
#endif
    return path;
}


std::unordered_map<bond::Slot, t_string> bond::swap_map(const std::unordered_map<t_string, bond::Slot> &map) {
    std::unordered_map<Slot, t_string> res;
    for (auto const &[key, value]: map) {
        res[value] = key;
    }
    return res;
}
