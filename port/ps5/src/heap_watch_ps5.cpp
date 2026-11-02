// Xerenge PS5: what is in memory when dlmalloc gives up. GPL-3.0-or-later.
//
// dlmalloc's corruption check calls abort, and the kernel's report has the
// registers but no memory. At that call rax holds the chunk it was unlinking
// (in the check that fires here, tmalloc_small's), so the wrap of abort keeps
// rax and dumps what lies before the chunk: the block that ran over it.
#include <cstdint>
#include <cstdio>

namespace ps5::startup
{
    void Say(const char* text);
}

extern "C"
{
    void __real_abort(void);
    uint64_t ps5_abort_rax;

    __attribute__((used)) void ps5_abort_report(void)
    {
        constexpr uintptr_t kHeapStart = 0x2000000000;
        const uintptr_t chunk = uintptr_t(ps5_abort_rax);
        char line[160];
        std::snprintf(line, sizeof(line), "[xerenge] abort: rax %#llx\n", (unsigned long long)chunk);
        ps5::startup::Say(line);
        if (chunk >= kHeapStart + 0x2000 && chunk < kHeapStart + (uintptr_t(1) << 34))
        {
            for (uintptr_t at = (chunk - 0x1800) & ~uintptr_t(31); at < chunk + 0x200; at += 32)
            {
                const auto* words = reinterpret_cast<const uint64_t*>(at);
                std::snprintf(line, sizeof(line), "[xerenge] %#llx %016llx %016llx %016llx %016llx\n",
                              (unsigned long long)at, (unsigned long long)words[0],
                              (unsigned long long)words[1], (unsigned long long)words[2],
                              (unsigned long long)words[3]);
                ps5::startup::Say(line);
            }
        }
        __real_abort();
        __builtin_trap();
    }

    __attribute__((naked)) void __wrap_abort(void)
    {
        __asm__("movq %rax, ps5_abort_rax(%rip)\n"
                "andq $-16, %rsp\n"
                "call ps5_abort_report\n");
    }
}
