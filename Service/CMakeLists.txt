#
# Copyright and description to be placed here
#

PROJECT(albaService)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS

	albaServiceLocator.cpp 	albaServiceLocator.h
	albaServiceClient.cpp 	albaServiceClient.h
	albaAbsLogicManager.h
)

SET(PROJECT_LIBS Libraries::wx::net Libraries::wx::core Libraries::wx::base)


# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})