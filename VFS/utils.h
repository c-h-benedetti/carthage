#ifndef GENERAL_UTILS_HPP_INCLUDED
#define GENERAL_UTILS_HPP_INCLUDED

#include <filesystem>
#include <stdexcept>

#include "SystemName.hpp"
#include "Name.hpp"
#include "FSType.hpp"
#include "Buffer.hpp"

#define CARTHAGE_DIR ".carthage"
#define VFS_DESCRIPTOR "vfs.ctg"

namespace fs = std::filesystem;

/// Position of an element in a file (shift from the begining).
using FSPos = uint64_t;
/// Size of a container, in number of blocks that it contains. Also size of a segment.
using FSize = uint32_t;
/// UID (user id, group id).
using UID = uint64_t;
/// Permissions over a block.
using Permissions = uint16_t;
/// Date of edition of a block.
using FSTime = fs::file_time_type;
/// Path compatible with all OS.
using Path = fs::path;

#define FIRST_BLOCK_POSITION sizeof(FSize)

int touch(const Path& path);

#endif // GENERAL_UTILS_HPP_INCLUDED