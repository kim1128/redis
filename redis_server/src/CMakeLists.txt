add_executable(redis_sev main.cpp common.cpp fork_pro.cpp thread_pro.cpp select_pro.cpp epoll_pro.cpp epoll_pro_ex.cpp)
target_link_libraries(redis_sev pthread)
#include_directories(../include)

#LINK_DIRECTORIES(pthread)
