#ifndef DY_PRECOMPILED_H
#define DY_PRECOMPILED_H

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if defined(__x86_64__)
#include <cpuid.h>
#endif

#ifdef __linux__

#include <link.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <pwd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/poll.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#endif

#if defined(__linux__) || defined(__APPLE__)
#include <syslog.h>
#endif

#if defined(_LP64) && defined(__x86_64__)
extern "C" {
#include <x86intrin.h>
}
#endif

#include <assert.h>
#include <iomanip>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <algorithm>
#include <array>
#include <ctime>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <inttypes.h>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef __GNUC__
#define GCC_VER ((__GNUC__ << 8) | __GNUC_MINOR__)
#endif

#if defined(__GNUC__) && (GCC_VER < 0x0406)
#include <stdatomic.h>
#else
#include <atomic>
#endif

#if defined(__linux__) && !defined(_GLIBCXX_USE_SCHED_YIELD)
#define _GLIBCXX_USE_SCHED_YIELD
#endif

#if defined(__linux__) && !defined(_GLIBCXX_USE_NANOSLEEP)
#define _GLIBCXX_USE_NANOSLEEP
#endif

#include <chrono>
#include <condition_variable>
#include <cxxabi.h>
#include <dirent.h>
#include <mutex>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <thread>
#include <unistd.h>

#ifdef __APPLE__
#include <arpa/inet.h>
#include <execinfo.h>
#include <libkern/OSAtomic.h>
#include <mach/mach_time.h>
#include <malloc/malloc.h>
#include <netinet/in.h>
#include <poll.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <malloc.h>
#include <semaphore.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <dlfcn.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <unwind.h>

#ifdef __aarch64__
#include <arm_neon.h>
#endif

#include <openssl/aes.h>
#include <openssl/asn1.h>
#include <openssl/bn.h>
#include <openssl/cmac.h>
#include <openssl/crypto.h>
#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/objects.h>
#include <openssl/param_build.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#endif  // DY_PRECOMPILED_H
