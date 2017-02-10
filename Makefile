all:
	g++ -O2 -std=c++14 -Wall -Wextra -pedantic \
-I. -I/usr/include/freetype2 \
\
main.cpp glad/glad.c app.cpp opengl/texture.cpp opengl/shader.cpp \
renderer_2d.cpp opengl/buffers.cpp opengl/sampler.cpp postprocessor.cpp \
text.cpp menu.cpp particle.cpp game/game.cpp game/paddle.cpp game/ball.cpp \
game/animation.cpp game/entity.cpp game/healthbar.cpp game/animentity.cpp \
game/collisions.cpp game/en_text.cpp game/powerup.cpp \
\
-lglfw -lGL -ldl -lfreetype -lsfml-audio \
-o demo
