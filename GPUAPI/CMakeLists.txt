PROJECT(GPUAPI)

# Set your list of sources here.
SET(PROJECT_SRCS
	GPU_OGL.cpp
	GPU_OGL.h
	FBO/framebufferObject.cpp
	FBO/framebufferObject.h
	FBO/glErrorUtil.cpp
	FBO/glErrorUtil.h
	FBO/renderbuffer.cpp
	FBO/renderbuffer.h
	GLEW/glew.h
	GLEW/glxew.h
	GLEW/wglew.h
  GLEW/glew.c
)

# List libraries that are needed by this project.
# SET(PROJECT_LIBS "mafVME" "mafInteraction" "vtkMAF")

# SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

