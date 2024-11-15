# Learn OpenGL

2024.10~?  OpenGL Learning + Minecraft mini renderer



## Requirements

- GLFW
- GLEW
- GLM

这些不在项目中包含，需要手动包含或在 vcpkg 中安装。

```powershell
./vcpkg install glfw3 glew glm imgui[glfw-binding,opengl3-binding,docking-experimental]
```

在 vcpkg 安装以上依赖库后，使用 vs2022 打开 LearnOpenGL.sln 即可。

TODO: 多平台支持/CMake



## OpenGL mini minecraft

OpenGL学习教程：[Cherno的Youtube List](https://www.youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2)、[汉化](https://www.bilibili.com/video/BV1Ni4y1o7Au)、[LearnOpenGL](https://learnopengl-cn.github.io/)

Minecraft 世界生成：[指路](https://izzel.io/2018/08/04/write-a-world-generator/)、[各种噪声介绍](https://www.cnblogs.com/xiaowangba/p/6314642.html)、[水流模拟](https://slembcke.github.io/WaterWaves)

Rendering 相关文章：[球面均匀采样](https://www.bogotobogo.com/Algorithms/uniform_distribution_sphere.php)、[Spherical Harmonic Lighting细节](https://www.cse.chalmers.se/~uffe/xjobb/Readings/GlobalIllumination/Spherical%20Harmonic%20Lighting%20-%20the%20gritty%20details.pdf)、[Tonemapping](https://www.cnblogs.com/uwatech/p/17960289)



