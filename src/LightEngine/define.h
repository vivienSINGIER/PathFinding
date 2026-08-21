#ifndef LIGHT_2D_DEFINE_H
#define LIGHT_2D_DEFINE_H

#define NOMINMAX
#include <windows.h>
#include <string>

// Returns the exe's own directory (no trailing slash)
inline std::string GetExeDir()
{
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string path(buffer, len);
    size_t pos = path.find_last_of("\\/");
    return (pos == std::string::npos) ? "" : path.substr(0, pos);
}

// Cheap existence check - no need to open the folder, just query its attributes
inline bool DirectoryExists(const std::string& path)
{
    DWORD attrs = GetFileAttributesA(path.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES) && (attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// Resolves the correct base path to res/, whether running under the
// debugger (IDE working directory) or standalone (exe's own folder)
#define RES_PATH_FALLBACK "../../"   // renamed from the old RES_PATH
#define RES_PATH ResolveResPath() +  // call sites keep using this

inline std::string FindResPath(const std::string& startDir)
{
    std::string dir = startDir;
    for (int i = 0; i < 6; ++i) // reasonable upper bound on how far up to search
    {
        if (DirectoryExists(dir + "\\res"))
            return dir + "\\";

        size_t pos = dir.find_last_of("\\/");
        if (pos == std::string::npos) break;
        dir = dir.substr(0, pos);
    }
    return startDir + "\\"; // give up, return original as before
}

inline std::string ResolveResPath()
{
    return FindResPath(GetExeDir());
}

#endif
