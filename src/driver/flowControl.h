#pragma once

// This nonsense right here is because SEH and C++ scope unwind in kernel-mode code don't play well together. Because we
// cannot build with /EH options with /kernel, object dtors do not get called. So instead of using SEH to exit the scope
// and perform cleanup, we use a while loop and breaks.

#define TRY do

#define FINALLY while (false);

#define LEAVE_IF_NOT_SUCCESS(r) \
    if (!NT_SUCCESS(r)) break;

#define LEAVE_WITH(s) \
    {                 \
        s;            \
        break;        \
    }

#define LEAVE() break;