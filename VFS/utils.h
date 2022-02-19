#ifndef GENERAL_UTILS_HPP_INCLUDED
#define GENERAL_UTILS_HPP_INCLUDED

#include <filesystem>

#include "SystemName.hpp"
#include "Name.hpp"
#include "FSType.hpp"
#include "Buffer.hpp"

#define CARTHAGE_DIR ".carthage"
#define VFS_DESCRIPTOR "vfs.ctg"

namespace fs = std::filesystem;

using FSPos = uint64_t;
using FSize = uint32_t;
using UID = uint64_t;
using Permissions = uint16_t;
using FileType = uint8_t;
using FSTime = fs::file_time_type;
using Path = fs::path;

#endif // GENERAL_UTILS_HPP_INCLUDED