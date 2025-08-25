/*
  Modern Chess Engine - Turkish Chess Engine
  Miscellaneous utilities implementation
*/

#include "misc.h"
#include <iostream>
#include <fstream>

namespace Chess {

void prefetch(void* addr) {
#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
  _mm_prefetch((char*)addr, _MM_HINT_T0);
#else
  __builtin_prefetch(addr);
#endif
}

void start_logger(const std::string& fname) {
    static std::ofstream log(fname, std::ofstream::out | std::ofstream::app);
    if (!log.is_open()) {
        std::cerr << "Failed to open log file: " << fname << std::endl;
    }
}

void* std_aligned_alloc(size_t alignment, size_t size) {
#if defined(_WIN32)
    return _aligned_malloc(size, alignment);
#else
    void* mem;
    return posix_memalign(&mem, alignment, size) ? nullptr : mem;
#endif
}

void std_aligned_free(void* ptr) {
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

void* aligned_large_pages_alloc(size_t size) {
    return std_aligned_alloc(64, size);
}

void aligned_large_pages_free(void* mem) {
    std_aligned_free(mem);
}

void dbg_hit_on(bool b) {
    // Debug statistics implementation
    static int count = 0;
    static int total = 0;
    if (b) count++;
    total++;
}

void dbg_hit_on(bool c, bool b) {
    if (c) dbg_hit_on(b);
}

void dbg_mean_of(int v) {
    // Debug mean calculation
    static int count = 0;
    static long long sum = 0;
    sum += v;
    count++;
}

void dbg_print() {
    // Print debug statistics
}

std::ostream& operator<<(std::ostream& os, SyncCout sc) {
    static std::mutex m;
    
    if (sc == IO_LOCK)
        m.lock();
    
    if (sc == IO_UNLOCK)
        m.unlock();
    
    return os;
}

namespace WinProcGroup {
void bindThisThread(size_t idx) {
    // Windows processor group binding
    (void)idx; // Suppress unused parameter warning
}
}

} // namespace Chess