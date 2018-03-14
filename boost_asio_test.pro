TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++17

#QMAKE_LFLAGS += -static -static-libstdc++ -static-libgcc

SOURCES += main.cpp \
    tcp_server.cpp \
    tcp_connection.cpp \
    console_handler.cpp

LIBS += -lboost_system -lpthread

HEADERS += \
    tcp_server.hpp \
    tcp_connection.hpp \
    console_handler.hpp \
    demangle.h

DEFINES += BOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS BOOST_ASIO_HAS_MOVE

#TODO:
# - custom handler allocator
# - use raw array instead of vector
