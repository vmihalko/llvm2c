#include "../core/Program.h"
#include "../core/Func.h"
#include "../core/Block.h"

#include <llvm/IR/Instruction.h>

#include <regex>

const std::set<std::string> C_FUNCTIONS = {"memcpy", "memmove", "memset", "sqrt", "powi", "sin", "cos", "pow", "exp", "exp2", "log", "log10", "log2",
                                           "fma", "fabs", "minnum", "maxnum", "minimum", "maximum", "copysign", "floor", "ceil", "trunc", "rint", "nearbyint",
"round", "va_start", "va_end", "va_copy"};


const std::set<std::string> C_MATH = {"sqrt", "powi", "sin", "cos", "pow", "exp", "exp2", "log", "log10", "log2",
                                      "fma", "fabs", "minnum", "maxnum", "minimum", "maximum", "copysign", "floor", "ceil", "trunc", "rint", "nearbyint",
                                      "round"};


const static std::set<std::string> STDLIB_FUNCTIONS = {"atof", "atoi", "atol", "strtod", "strtol", "strtoul", "calloc",
                                                       "free", "malloc", "realloc", "abort", "atexit", "exit", "getenv",
                                                       "system", "bsearch", "qsort", "abs", "div", "labs", "ldiv",
                                                       "rand", "srand", "mblen", "mbstowcs", "mbtowc", "wcstombs", "wctomb",
                                                       "strtoll", "strtoull", "realpath"};

const static std::set<std::string> STRING_FUNCTIONS = {"memchr", "memcmp", "memcpy", "memmove", "memset", "strcat", "strncat",
                                                       "strchr", "strcmp", "strncmp", "strcoll", "strcpy", "strncpy", "strcspn",
                                                       "strerror", "strlen", "strpbrk", "strrchr", "strspn", "strstr", "strtok",
                                                       "strxfrm", "strsep", "strnlen", "strncasecmp", "strcasecmp", "stpcpy",
                                                       "strdup"};

const static std::set<std::string> STDIO_FUNCTIONS = {"fclose", "clearerr", "feof", "ferror", "fflush", "fgetpos", "fopen",
                                                      "fread", "freopen", "fseek", "fsetpos", "ftell", "fwrite", "remove",
                                                      "rename", "rewind", "setbuf", "setvbuf", "tmpfile", "tmpnam", "fprintf",
                                                      "sprintf", "vfprintf", "vsprintf", "fscanf", "scanf", "sscanf", "fgetc",
                                                      "fgets", "fputc", "fputs", "getc", "getchar", "gets", "putc", "putchar",
                                                      "puts", "ungetc", "perror", "snprintf", "vsnprintf", "printf", "pclose",
                                                      "popen", "fileno", "fseeko"};

const static std::set<std::string> PTHREAD_FUNCTIONS = {"pthread_attr_destroy", "pthread_attr_getdetachstate", "pthread_attr_getguardsize",
                                                        "pthread_attr_getinheritsched", "pthread_attr_getschedparam", "pthread_attr_getschedpolicy",
                                                        "pthread_attr_getscope", "pthread_attr_getstackaddr", "pthread_attr_getstacksize",
                                                        "pthread_attr_init", "pthread_attr_setdetachstate", "pthread_attr_setguardsize",
                                                        "pthread_attr_setinheritsched", "pthread_attr_setschedparam", "pthread_attr_setschedpolicy",
                                                        "pthread_attr_setscope", "pthread_attr_setstackaddr", "pthread_attr_setstacksize",
                                                        "pthread_cancel", "pthread_cleanup_push", "pthread_cleanup_pop", "pthread_cond_broadcast",
                                                        "pthread_cond_destroy", "pthread_cond_init", "pthread_cond_signal", "pthread_cond_timedwait",
                                                        "pthread_cond_wait", "pthread_condattr_destroy", "pthread_condattr_getpshared",
                                                        "pthread_condattr_init", "pthread_condattr_setpshared","pthread_create",  "pthread_detach",
                                                        "pthread_equal", "pthread_exit", "pthread_getconcurrency", "pthread_getschedparam","pthread_getspecific",
                                                        "pthread_join", "pthread_key_create", "pthread_key_delete", "pthread_mutex_destroy",
                                                        "pthread_mutex_getprioceiling", "pthread_mutex_init", "pthread_mutex_lock",
                                                        "pthread_mutex_setprioceiling", "pthread_mutex_trylock", "pthread_mutex_unlock",
                                                        "pthread_mutexattr_destroy", "pthread_mutexattr_getprioceiling", "pthread_mutexattr_getprotocol",
                                                        "pthread_mutexattr_getpshared", "pthread_mutexattr_gettype", "pthread_mutexattr_init",
                                                        "pthread_mutexattr_setprioceiling", "pthread_mutexattr_setprotocol", "pthread_mutexattr_setpshared",
                                                        "pthread_mutexattr_settype", "pthread_once","pthread_rwlock_destroy", "pthread_rwlock_init",
                                                        "pthread_rwlock_rdlock", "pthread_rwlock_tryrdlock", "pthread_rwlock_trywrlock",
                                                        "pthread_rwlock_unlock", "pthread_rwlock_wrlock","pthread_rwlockattr_destroy",
                                                        "pthread_rwlockattr_getpshared", "pthread_rwlockattr_init", "pthread_rwlockattr_setpshared",
                                                        "pthread_self","pthread_setcancelstate",  "pthread_setcanceltype", "pthread_setconcurrency",
                                                        "pthread_setschedparam", "pthread_setspecific", "pthread_testcancel"};

bool isCFunc(const std::string& func) {
    return C_FUNCTIONS.find(func) != C_FUNCTIONS.end();
}

bool isStdLibFunc(const std::string& func) {
    return STDLIB_FUNCTIONS.find(func) != STDLIB_FUNCTIONS.end();
}

bool isStringFunc(const std::string& func) {
    return STRING_FUNCTIONS.find(func) != STRING_FUNCTIONS.end();
}

bool isStdioFunc(const std::string& func) {
    return STDIO_FUNCTIONS.find(func) != STDIO_FUNCTIONS.end();
}

bool isPthreadFunc(const std::string& func) {
    return PTHREAD_FUNCTIONS.find(func) != PTHREAD_FUNCTIONS.end();
}

bool isCMath(const std::string& func) {
    return C_MATH.find(func) != C_MATH.end();
}

std::string trimPrefix(const std::string& func) {
    std::regex function("llvm\\.(\\w+)(\\..+){0,1}");
    std::smatch match;
    if (std::regex_search(func, match, function)) {
        return match[1].str();
    }

    return "";
}


void determineIncludes(const llvm::Module* module, Program& program) {
    for (const llvm::Function& func : module->functions()) {
        std::string name = func.getName().str();
        std::string trimmed = trimPrefix(name);

        if (isCFunc(trimmed)) {
            name = trimmed;
        }

        if (isStdLibFunc(name)) {
            program.hasStdLib = true;
        }

        if (isStringFunc(name)) {
            program.hasString = true;
        }

        if (isStdioFunc(name)) {
            program.hasStdio = true;
        }

        if (isPthreadFunc(name)) {
            program.hasPthread = true;
        }

    }

    program.addPass(PassType::DetermineIncludes);
}
