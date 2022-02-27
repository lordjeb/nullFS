#pragma once

namespace Logging
{
    TRACELOGGING_DECLARE_PROVIDER(TraceLoggingProviderHandle);

    enum class Keyword : unsigned long long {
        Common = (1 << 0),
        FsCtrl = (1 << 1),
        Create = (1 << 2),
        Close = (1 << 3),
        Cleanup = (1 << 4),
        Pnp = (1 << 5),
        Shutdown = (1 << 6),
        DeviceControl = (1 << 7),
    };
}

#define NfTraceCommon(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                               \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Common), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceFsCtrl(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                               \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::FsCtrl), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceCreate(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                               \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Create), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceClose(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                              \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Close), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceCleanup(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                                \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Cleanup), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTracePnp(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                            \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Pnp), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceShutdown(level, taskname, ...)                                                                        \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                                                 \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::Shutdown), TraceLoggingLevel(level), \
                      __VA_ARGS__);

#define NfTraceDeviceControl(level, taskname, ...)                                              \
    TraceLoggingWrite(Logging::TraceLoggingProviderHandle, taskname,                            \
                      TraceLoggingKeyword((unsigned long long)Logging::Keyword::DeviceControl), \
                      TraceLoggingLevel(level), __VA_ARGS__);
