TEMPLATE = app
CONFIG += c++14 strict_c++
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -pedantic

LIBS += -lglfw -lGL -ldl -lfreetype -lsfml-audio

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
    menu.cpp \
    particle.cpp \
    game/game.cpp \
    game/paddle.cpp \
    game/ball.cpp \
    game/animation.cpp \
    game/entity.cpp \
    game/healthbar.cpp \
    game/animentity.cpp \
    game/collisions.cpp \
    game/en_text.cpp \
    game/powerup.cpp

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
    menu.hpp \
    soundsystem.hpp \
    particle.hh \
    game/game.hpp \
    game/entity.hpp \
    game/paddle.hpp \
    game/ball.hpp \
    game/brick.hpp \
    game/animation.hh \
    game/healthbar.hpp \
    game/animentity.hpp \
    game/collisions.hpp \
    game/en_text.hpp \
    game/powerup.hpp
