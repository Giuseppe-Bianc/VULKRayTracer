del /s /q shaders\vert.spv
del /s /q shaders\frag.spv

C:\dev\VulkanSDK\1.3.246.1\Bin\glslc.exe shaders\vert.vert -o shaders\vert.spv
C:\dev\VulkanSDK\1.3.246.1\Bin\glslc.exe shaders\frag.frag -o shaders\frag.spv