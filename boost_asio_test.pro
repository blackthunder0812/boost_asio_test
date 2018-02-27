TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tcp_server.cpp \
    tcp_connection.cpp

LIBS += -lboost_system -lpthread

HEADERS += \
    tcp_server.hpp \
    tcp_connection.hpp
