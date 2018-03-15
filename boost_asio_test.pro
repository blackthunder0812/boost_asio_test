TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++17

#QMAKE_LFLAGS += -static -static-libstdc++ -static-libgcc

SOURCES += main.cpp \
    tcp_server.cpp \
    tcp_connection.cpp \
    console_handler.cpp \
    handler_memory.cpp

LIBS += -lboost_system -lpthread

HEADERS += \
    tcp_server.hpp \
    tcp_connection.hpp \
    console_handler.hpp \
    demangle.h \
    handler_memory.hpp \
    handler_allocator.hpp \
    allocator_handler.hpp

DEFINES += BOOST_ASIO_DISABLE_HANDLER_TYPE_REQUIREMENTS BOOST_ASIO_HAS_MOVE

#TODO:
# - use raw array instead of vector
