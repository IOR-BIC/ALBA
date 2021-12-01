/*=========================================================================

Program: ALBA
Module: albaGPU3DTextureProviderHelper.h
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGPU3DTextureProviderHelper_H__
#define __albaGPU3DTextureProviderHelper_H__


#include <vector>

//#include "albaDefines.h"
class vtkImageData;
class albaGPUOGL;

struct TextureList
{
	int m_TexetureID;
	vtkImageData *m_Volume;
	albaGPUOGL* m_pGPUProvider;    //<GPU provider for GPU computation
	float m_DataDims[3]; //<area covered by input data (in mm)
	int m_ReferenceCount;
};


class albaGPU3DTextureProviderHelper 
{
public:
	albaGPU3DTextureProviderHelper();
	~albaGPU3DTextureProviderHelper();

	int InitializeTexture(vtkImageData *volume, vtkImageData *output);

	void UnregisterTexture();

	void CreateImage(vtkImageData *outputObject, float *globPlaneX, float *globPlaneY, float *globPlaneZ, double *globPlaneOrigin);
protected:
	
	/** Slices voxels from input producing image in output using GPU. */
	template<typename OutputDataType>
	void CreateImage(OutputDataType* output, vtkImageData *outputObject, float *globPlaneX, float *globPlaneY, float *globPlaneZ, double *globPlaneOrigin);

	/** return the position of the texture in the TextureList vector*/
	int GetTextureIndex(vtkImageData *volume);
	/** return the position of the texture in the TextureList vector*/
	int GetTextureIndex(int textureId);

	/** Returns a texture id from a vtk dataset, -1 if the the volume is not already registered*/
	int GetTextureId(vtkImageData *volume);

	/** Gets the GL data type (to be used for albaGPU_OGL) for the given VTK data type
	Returns -1, if the given data type has no equivalent in OpenGL*/
	unsigned int GetGLDataType(int nVTKdata_type);

	/** Creates GPU provider, shaders, etc.
	Returns false, if GPU provider could not be created */
	albaGPUOGL *CreateGPUProvider();

	unsigned int m_TextureId;
};


#endif