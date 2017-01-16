#
# Copyright and description to be placed here
#

PROJECT(mafService)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS

	mafServiceLocator.cpp 	mafServiceLocator.h
	mafServiceClient.cpp 	mafServiceClient.h
	mafAbsLogicManager.h
)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})