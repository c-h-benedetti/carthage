#ifndef GENERAL_UTILS_HPP_INCLUDED
#define GENERAL_UTILS_HPP_INCLUDED

#include <filesystem>
#include <cstdint>

#include "ID.hpp"
#include "Name.hpp"
#include "FSType.hpp"

#define CARTHAGE_DIR ".carthage"
#define VFS_DESCRIPTOR "vfs_descriptor.ctg"

namespace fs = std::filesystem;

using FPos = uint64_t;
using FSize = uint32_t;
using UID = uint32_t;
using Permissions = uint16_t;
using FileType = uint16_t;
using FSTime = fs::file_time_type;
using Path = fs::path;

#endif // GENERAL_UTILS_HPP_INCLUDED