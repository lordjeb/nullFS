#pragma once

namespace Logging
{
    TRACELOGGING_DECLARE_PROVIDER(g_hProvider);

    enum class Keyword : unsigned long long {
        Common = (1 << 0),
        VolumeMount = (1 << 1),
        Create = (1 << 2),
        Close = (1 << 3),
        Cleanup = (1 << 4)
    };
}

#define NfTraceCommon(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::g_hProvider, taskname,                                                              \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Common), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceVolumeMount(level, taskname, ...)                                              \
    TraceLoggingWrite(Logging::g_hProvider, taskname,                                         \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::VolumeMount), \
                      TraceLoggingLevel(level), __VA_ARGS__);

#define NfTraceCreate(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::g_hProvider, taskname,                                                              \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Create), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceClose(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::g_hProvider, taskname,                                                              \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Close), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceCleanup(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::g_hProvider, taskname,                                                              \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Cleanup), TraceLoggingLevel(level), \
                      __VA_ARGS__);
