#pragma once

#define MOTI_VA_ARGS_PASS(...) MOTI_VA_ARGS_PASS_1_ __VA_ARGS__ MOTI_VA_ARGS_PASS_2_
#define MOTI_VA_ARGS_PASS_1_ (
#define MOTI_VA_ARGS_PASS_2_ )

#define MOTI_VA_ARGS_COUNT(...) MOTI_VA_ARGS_COUNT_ MOTI_VA_ARGS_PASS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define MOTI_VA_ARGS_COUNT_(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, last, ...) last

#define MOTI_MACRO_DISPATCHER(p_func, ...) MOTI_MACRO_DISPATCHER_1_(p_func, MOTI_VA_ARGS_COUNT(__VA_ARGS__) )
#define MOTI_MACRO_DISPATCHER_1_(p_func, p_argc) MOTI_MACRO_DISPATCHER_2_(p_func, p_argc)
#define MOTI_MACRO_DISPATCHER_2_(p_func, p_argc) MOTI_CONCATENATE(p_func, p_argc)

#define MOTI_CONCATENATE(p_x, p_y) MOTI_CONCATENATE_(p_x, p_y)
#define MOTI_CONCATENATE_(p_x, p_y) p_x ## p_y

#define MOTI_TO_STRING(p_x) MOTI_TO_STRING_(p_x)
#define MOTI_TO_STRING_(p_x) #p_x


#define MOTI_FORWARD_DECLARE_1(c) class c;
#define MOTI_FORWARD_DECLARE_2(n1, c) namespace n1 { MOTI_FORWARD_DECLARE_1(c) }
#define MOTI_FORWARD_DECLARE_3(n1, n2, c) namespace n1 { MOTI_FORWARD_DECLARE_2(n2, c) }
#define MOTI_FORWARD_DECLARE_4(n1, n2, n3, c) namespace n1 { MOTI_FORWARD_DECLARE_3(n2, n3, c) }
#define MOTI_FORWARD_DECLARE_5(n1, n2, n3, n4, c) namespace n1 { MOTI_FORWARD_DECLARE_4(n2, n3, n4, c) }

#define MOTI_FORWARD_DECLARE_STRUCT_1(c) struct c;
#define MOTI_FORWARD_DECLARE_STRUCT_2(n1, c) namespace n1 { MOTI_FORWARD_DECLARE_STRUCT_1(c) }
#define MOTI_FORWARD_DECLARE_STRUCT_3(n1, n2, c) namespace n1 { MOTI_FORWARD_DECLARE_STRUCT_2(n2, c) }
#define MOTI_FORWARD_DECLARE_STRUCT_4(n1, n2, n3, c) namespace n1 { MOTI_FORWARD_DECLARE_STRUCT_3(n2, n3, c) }
#define MOTI_FORWARD_DECLARE_STRUCT_5(n1, n2, n3, n4, c) namespace n1 { MOTI_FORWARD_DECLARE_STRUCT_4(n2, n3, n4, c) }

#define MOTI_FORWARD_DECLARE(...) MOTI_MACRO_DISPATCHER(MOTI_FORWARD_DECLARE_, __VA_ARGS__) MOTI_VA_ARGS_PASS(__VA_ARGS__)
#define MOTI_FORWARD_DECLARE_STRUCT(...) MOTI_MACRO_DISPATCHER(MOTI_FORWARD_DECLARE_STRUCT_, __VA_ARGS__) MOTI_VA_ARGS_PASS(__VA_ARGS__)