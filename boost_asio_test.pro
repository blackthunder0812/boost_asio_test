TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++17

SOURCES += main.cpp \
    tcp_server.cpp \
    tcp_connection.cpp \
    console_handler.cpp

LIBS += -lboost_system

HEADERS += \
    tcp_server.hpp \
    tcp_connection.hpp \
    console_handler.hpp \
    demangle.h
