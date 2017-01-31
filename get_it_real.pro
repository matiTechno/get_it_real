TEMPLATE = app
CONFIG += c++14 strict_c++
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -pedantic

LIBS += -lglfw -lGL -ldl -lfreetype

INCLUDEPATH += /usr/include/freetype2

SOURCES += main.cpp \
    glad/glad.c \
    app.cpp \
    opengl/texture.cpp \
    opengl/shader.cpp \
    renderer_2d.cpp \
    opengl/buffers.cpp \
    opengl/sampler.cpp \
    postprocessor.cpp \
    text.cpp \
    menu.cpp

HEADERS += \
    glad/glad.h \
    app.hpp \
    opengl/texture.hpp \
    opengl/stb_image.h \
    sprite.hpp \
    opengl/shader.hpp \
    renderer_2d.hpp \
    opengl/buffers.hpp \
    opengl/sampler.hpp \
    postprocessor.hpp \
    text.hpp \
    input.hpp \
    menu.hpp
