# arkanoid_complete

![](https://github.com/matiTechno/get_it_real/blob/master/screens/screen3.png)

dependencies (headers and libs):
* [sfml-audio](https://github.com/SFML/SFML)
* [GLM](https://github.com/g-truc/glm) (this is only header library)
* [GLFW](https://github.com/glfw/glfw)
* [FreeType](https://www.freetype.org)

[stb_image.h](https://github.com/nothings/stb) and files generated by [glad](https://github.com/Dav1dde/glad) are included in this project.  
All of the art and sound effects comes from [opengameart.org](http://opengameart.org).  
It compiles on both Linux, Windows and OS X.  

### building
Linux:
* sudo apt install libglfw3-dev libfreetype6-dev libsfml-dev libglm-dev
* $ make

For Windows and Mac follow build guides (those are very well written) for sfml, glfw, and freetype on their webpages. 
On Windows you can use binary included in this project (it must be in the same directory as res dir).

### resolution
Default window size is 640x480 but you can change it by passing new width and height as arguments when invoking program (keep the aspect ratio).
It is not possible to change resolution during runtime due to my postprocessing system inflexibility (but there is an easy fix and it will be implemented in future projects).

### performance
Debugging with [apitrace](https://github.com/apitrace/apitrace) did not show any bottlenecks. On linux with Intel HD graphics 520 it runs
with 240 fps (4.2 ms per frame)(900x700 framebuffer). Postprocessing (bloom mostly) takes most of the frametime.
In future projects i will definitely use instanced rendering for sprites to limit opengl api calls. I will also consider using glMapBufferRange instead
of glBufferSubData when rendering particles (or even move them to gpu with transform feedback or compute shader).
