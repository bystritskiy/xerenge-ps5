// Xerenge PS5: the bytes of the code at an address. GPL-3.0-or-later.
//
// The SDK's MMIO handler decodes the instruction that faulted, and eboot code
// is execute-only on the console: reading it faults as well. The same bytes are
// in /app0/eboot.bin, the SELF entry of segment 0, stored plain. An
// instruction read with pread costs no memory, and pread is safe in the handler.
//
// ps5-native-tool writes the PS4 FSELF magic 0x1D3D154F; fPKG builders rewrite
// it to the PS5 magic 0xEEF51454 and keep the header and segment table.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace
{
    struct ModuleSegment
    {
        uint64_t address;
        uint32_t size;
        int32_t protection;
    };
    struct ModuleInfo
    {
        uint64_t size;
        char name[256];
        ModuleSegment segments[4];
        uint32_t count;
        uint8_t fingerprint[20];
    };
    static_assert(sizeof(ModuleInfo) == 0x160, "SceKernelModuleInfo is 0x160 bytes");

    int s_file = -1;
    uint64_t s_start = 0, s_size = 0, s_offset = 0;
}

extern "C"
{
    int sceKernelGetExecutableModuleHandle(void);
    int sceKernelGetModuleInfo(int handle, ModuleInfo* info);

    // False when the address is not in the code segment, or nothing could be read.
    bool ps5_code_bytes(uint64_t address, uint8_t* out, size_t count)
    {
        if (s_file < 0 || address < s_start || address - s_start >= s_size)
            return false;
        const uint64_t at = address - s_start;
        const size_t wanted = at + count > s_size ? size_t(s_size - at) : count;
        std::memset(out, 0, count);
        return pread(s_file, out, wanted, off_t(s_offset + at)) == ssize_t(wanted);
    }
}

namespace ps5::startup
{
    void FindCode()
    {
        ModuleInfo info;
        std::memset(&info, 0, sizeof(info));
        info.size = sizeof(info);
        const int handle = sceKernelGetExecutableModuleHandle();
        const int rc = sceKernelGetModuleInfo(handle, &info);
        const uintptr_t self = reinterpret_cast<uintptr_t>(&ps5_code_bytes);
        uint64_t start = 0, limit = 0;
        for (uint32_t i = 0; rc == 0 && i < info.count && i < 4; i++)
        {
            const ModuleSegment& segment = info.segments[i];
            if (self >= segment.address && self - segment.address < segment.size)
            {
                start = segment.address;
                limit = segment.size;
            }
        }

        struct
        {
            uint32_t magic;
            uint8_t version, mode, endian, attributes;
            uint32_t key;
            uint16_t header, meta;
            uint64_t file;
            uint16_t entries, flags;
            uint32_t pad;
        } header{};
        struct
        {
            uint64_t properties, offset, file, memory;
        } entry;
        const int file = start ? open("/app0/eboot.bin", O_RDONLY) : -1;
        uint64_t offset = 0, size = 0;
        if (file >= 0 && pread(file, &header, sizeof(header), 0) == ssize_t(sizeof(header)) &&
            (header.magic == 0x1d3d154fu || header.magic == 0xeef51454u))
        {
            for (unsigned i = 0; i < header.entries; i++)
            {
                if (pread(file, &entry, sizeof(entry), 0x20 + 0x20 * off_t(i)) != ssize_t(sizeof(entry)))
                    break;
                // Bits 20 and up: the segment; bit 1: encrypted; bit 3: compressed.
                // The module's segment is the file size rounded up to 16 KiB.
                if ((entry.properties >> 20) == 0 && !(entry.properties & 0xa) &&
                    entry.file <= limit && limit - entry.file < 0x4000)
                {
                    offset = entry.offset;
                    size = entry.file;
                    break;
                }
            }
        }
        std::fprintf(stderr, "[ps5] code for MMIO decoding: module %#x (%#x), segment %#llx+%#llx, "
                     "eboot.bin magic %#x, file offset %#llx, %llu bytes\n",
                     unsigned(handle), unsigned(rc), (unsigned long long)start, (unsigned long long)limit,
                     header.magic, (unsigned long long)offset, (unsigned long long)size);
        if (!offset)
        {
            if (file >= 0)
                close(file);
            return;
        }
        s_start = start;
        s_size = size;
        s_offset = offset;
        s_file = file;
    }
}
