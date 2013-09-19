#ifndef _MACROS_HH_
#define _MACROS_HH_

// Helper Macros for operation registration

#define ANSWER_CONCATENATE_DETAIL(x, y) x##y
#define ANSWER_CONCATENATE(x, y) ANSWER_CONCATENATE_DETAIL(x, y)
#define ANSWER_MAKE_UNIQUE(x) ANSWER_CONCATENATE(x, __COUNTER__)

#endif // _MACROS_HH_