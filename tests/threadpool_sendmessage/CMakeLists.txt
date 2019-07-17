add_executable(echoServer EchoServer.cc)
target_link_libraries(echoServer muduo_net)


add_executable(echoClient EchoClient.cc)
target_link_libraries(echoClient muduo_net)