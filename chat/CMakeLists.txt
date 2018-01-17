include_directories(/home/berlin/code/github/build/release-install/include)

link_directories(/home/berlin/code/github/build/release-install/lib)

add_executable(asio_chat_client client.cc)
target_link_libraries(asio_chat_client muduo_base muduo_net pthread)

# add_executable(asio_chat_loadtest loadtest.cc)
# target_link_libraries(asio_chat_loadtest muduo_net)

add_executable(asio_chat_server server.cc)
target_link_libraries(asio_chat_server muduo_base muduo_net pthread)

# add_executable(asio_chat_server_threaded server_threaded.cc)
# target_link_libraries(asio_chat_server_threaded muduo_net)

# add_executable(asio_chat_server_threaded_efficient server_threaded_efficient.cc)
# target_link_libraries(asio_chat_server_threaded_efficient muduo_net)

# add_executable(asio_chat_server_threaded_highperformance server_threaded_highperformance.cc)
# target_link_libraries(asio_chat_server_threaded_highperformance muduo_net)
add_compile_options(-std=c++11)

include_directories(./../lua515)
include_directories(./../lua_tinker)

add_executable(luaserver luaserver.cpp)
target_link_libraries(luaserver lua luatinker)
target_link_libraries(luaserver muduo_base muduo_net pthread)
