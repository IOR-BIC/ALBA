
/*=========================================================================

Program: ALBA
Module: albaGPU3DTextureProviderHelper.cpp
Authors: Gianluigi Crimi	

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaGPU3DTextureProviderHelper.h"
#include "vtkImagedata.h"
#include "vtkRectilinearGrid.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include <Windows.h>
#include "GPU_OGL.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))


//Global vector with all current registered 3d texture 
std::vector<TextureList> glo_Textures;


//----------------------------------------------------------------------------
albaGPU3DTextureProviderHelper::albaGPU3DTextureProviderHelper()
{
	m_TextureId = 0;
}

//----------------------------------------------------------------------------
albaGPU3DTextureProviderHelper::~albaGPU3DTextureProviderHelper()
{
	//Unregister current texture if exist 
	UnregisterTexture();
}

//----------------------------------------------------------------------------
int albaGPU3DTextureProviderHelper::InitializeTexture(vtkImageData *volume, vtkImageData *output)
{

	
	int pos = GetTextureIndex(volume);
	if (pos != -1)
	{
		//texture already exists just increment the reference count and get current texture id and GPUprovider
		glo_Textures[pos].m_ReferenceCount++;
		m_TextureId = glo_Textures[pos].m_TexetureID;
		return true;
	}

	//Creating a new Texture in the GPU MEM
	//GPU processing is allowed by the caller, we have compatible input and output
	//thus we need to check, if we can process 

	TextureList tItem;
	tItem.m_ReferenceCount = 0;
	tItem.m_Volume = volume;

	//Creating a new provider and store it to the new Texture Item
	albaGPUOGL *gpuProvider;
	tItem.m_pGPUProvider = gpuProvider = CreateGPUProvider();

	if (gpuProvider == NULL)
		return false;  //Cannot Create the GPU Provider

	//GPU checks
	int inDType = volume->GetScalarType();
	int outDType = output->GetScalarType();

	unsigned int inGLDtype = GetGLDataType(inDType);
	unsigned int outGLDtype = GetGLDataType(outDType);

	int nComps = volume->GetNumberOfScalarComponents();
	
	//Check data types
	if (!(nComps == 1 || nComps == 3))
		return false;

	int maxDimGL;
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &maxDimGL);

	int *dims = volume->GetDimensions();
	double *spacing = volume->GetSpacing();

	//Storing Data Dimension to the Texture Item
	for (int i = 0; i < 3; i++)
		tItem.m_DataDims[i] = spacing[i]*(dims[i]-1);

	if (maxDimGL < max(dims[0], max(dims[1], dims[2])))
		return false; //unfortunately, 3D textures of this size are not supported

	//we need to create texture
	gpuProvider->EnableRenderingContext();

	//Unregister current texture if exist 
	UnregisterTexture();


	//Create a new texture and store tID to the new Item
	glGenTextures(1, (GLuint*)&m_TextureId);
	tItem.m_TexetureID = m_TextureId;

	glBindTexture(GL_TEXTURE_3D, m_TextureId);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	GLint iformat;
	if (inDType == VTK_CHAR || inDType == VTK_UNSIGNED_CHAR)
		iformat = GL_ALPHA;
	else if (inDType == VTK_UNSIGNED_SHORT)
		iformat = GL_ALPHA16;
	else
		iformat = GL_ALPHA32F_ARB;

	glTexImage3D(GL_TEXTURE_3D, 0, iformat, dims[0],dims[1], dims[2], 0,
		(nComps == 1 ? GL_ALPHA : GL_RGB), inGLDtype,
		volume->GetPointData()->GetScalars()->GetVoidPointer(0));


	if (glGetError() != GL_NO_ERROR)
	{
		glDeleteTextures(1, (GLuint*)&m_TextureId);
		m_TextureId = 0;
		return false;
	}

	gpuProvider->DisableRenderingContext();

	//Adding new item in the Texture List
	glo_Textures.push_back(tItem);

	return true;
}

//----------------------------------------------------------------------------
int albaGPU3DTextureProviderHelper::GetTextureIndex(vtkImageData *volume)
{
	std::vector<TextureList>::size_type textSize = glo_Textures.size();
	for (int i = 0; i < textSize; i++)
	{
		if (glo_Textures[i].m_Volume == volume)
			return i;
	}
	return -1;
}

//----------------------------------------------------------------------------
int albaGPU3DTextureProviderHelper::GetTextureIndex(int textureId)
{
	if (textureId == 0)
		return -1;

	std::vector<TextureList>::size_type textSize = glo_Textures.size();
	for (int i = 0; i < textSize; i++)
	{
		if (glo_Textures[i].m_TexetureID == textureId)
			return i;
	}
	return -1;
}
//----------------------------------------------------------------------------
int albaGPU3DTextureProviderHelper::GetTextureId(vtkImageData *volume)
{
	int pos = GetTextureIndex(volume);
		
	if (pos == -1)
		return -1;
	else 
		return glo_Textures[pos].m_TexetureID;
}

//----------------------------------------------------------------------------
void albaGPU3DTextureProviderHelper::UnregisterTexture()
{
	int pos = GetTextureIndex(m_TextureId);
	if (pos == -1)
		return;

	glo_Textures[pos].m_ReferenceCount--;

	//If the reference count is zero we need to delete the opengl texture and erase the entry in the list
	if (glo_Textures[pos].m_ReferenceCount == 0)
	{
		//Delete texture
		glo_Textures[pos].m_pGPUProvider->EnableRenderingContext();
		glDeleteTextures(1, (GLuint*)&m_TextureId);
		glo_Textures[pos].m_pGPUProvider->DisableRenderingContext();

		//Release GPU provider
		albaGPUOGL::ReleaseGPUOGL();

		//Erase the entry in the list:
		glo_Textures.erase(glo_Textures.begin() + pos);
	}

	m_TextureId = 0;
}

//------------------------------------------------------------------------
unsigned int albaGPU3DTextureProviderHelper::GetGLDataType(int nVTKdata_type)
{
	//see vtkSystemIncludes.h
	//#define VTK_VOID            0
	//#define VTK_BIT             1 
	//#define VTK_CHAR            2
	//#define VTK_UNSIGNED_CHAR   3
	//#define VTK_SHORT           4
	//#define VTK_UNSIGNED_SHORT  5
	//#define VTK_INT             6
	//#define VTK_UNSIGNED_INT    7
	//#define VTK_LONG            8
	//#define VTK_UNSIGNED_LONG   9
	//#define VTK_FLOAT          10
	//#define VTK_DOUBLE         11 
	//#define VTK_ID_TYPE        12
	const unsigned int RET_VALS[] = {
		(unsigned int)-1, (unsigned int)-1,
		GL_BYTE, GL_UNSIGNED_BYTE,
		GL_SHORT, GL_UNSIGNED_SHORT,
		(unsigned int)-1, GL_UNSIGNED_INT,
		GL_INT, GL_UNSIGNED_INT,
		GL_FLOAT, (unsigned int)-1, (unsigned int)-1,
	};

	if (nVTKdata_type > VTK_ID_TYPE)
		return (unsigned int)-1;
	else
		return RET_VALS[nVTKdata_type];
}


//----------------------------------------------------------------------------
albaGPUOGL *albaGPU3DTextureProviderHelper::CreateGPUProvider()
{
	
	if (!albaGPUOGL::IsSupported())
		return NULL;

	//supported => OK
	albaGPUOGL *gpuProvider = albaGPUOGL::BindGPUOGL();

	//create shader
	const char* ps =
		"uniform sampler3D voxels;   //input voxels data\n"
		"uniform vec3 dimension;     //area covered by data\n"
		"uniform vec3 pl_ofs;        //output texture origin\n"
		"uniform vec3 pl_x;          //output texture x-coord\n"
		"uniform vec3 pl_y;          //output texture y-coord\n"
		"\n"
		"void main(void) \n"
		"{\n"
		"  //compute the coordinates of current sample in the space\n"
		"  vec3 pos_in_space = pl_ofs + gl_FragCoord.x*pl_x + gl_FragCoord.y*pl_y;\n"
		"\n"
		"  //convert them into texture coordinates\n"
		"  vec3 tcoord = vec3(pos_in_space.x / dimension.x,\n"
		"    pos_in_space.y / dimension.y, \n"
		"    pos_in_space.z / dimension.z);\n"
		"\n"
		"  gl_FragColor = texture3D(voxels, tcoord);  \n"
		"}\n";


	wxString err;
	if (!gpuProvider->CreateShaders(NULL, ps, &err))
	{
		albaGPUOGL::ReleaseGPUOGL();
		return NULL;
	}

	return gpuProvider;
}

//----------------------------------------------------------------------------
template<typename OutputDataType>
void albaGPU3DTextureProviderHelper::CreateImage(OutputDataType* output, vtkImageData *outputObject, float *globPlaneX, float *globPlaneY, float *globPlaneZ, double *globPlaneOrigin)
{
	int pos = GetTextureIndex(m_TextureId);
	if (pos == -1)
		return;

	vtkImageData * volume = glo_Textures[pos].m_Volume;
	double *origin = volume->GetOrigin();

	// prepare data for sampling  
	const int xs = outputObject->GetDimensions()[0], ys = outputObject->GetDimensions()[1];
	const float dx = outputObject->GetSpacing()[0], dy = outputObject->GetSpacing()[1];
	const int numComp = volume->GetNumberOfScalarComponents();

	//assert(this->NumComponents == outputObject->GetNumberOfScalarComponents());

	//the first pixel of the texture, i.e., the pixel at [0,0], has coordinates globPlaneOrigin
	//the pixel [i+1,j] has coordinates equal to the coordinates of the pixel [i,j] + 
	//globPlaneX*dx; the pixel [i, j+1] has coordinates equal to the coordinates 
	//of the pixel [i,j] + globPlaneY*dy

	//if the scene is translated so the first voxel has coordinates [0.0, 0.0, 0.0]
	//the first pixel has coordinates globPlaneOrigin - DataOrigin
	//as there is SamplingTableMultiplier samples per one mm, pixel coordinates to 
	//sample indices can be simply obtained using these variables:
	float xaxis[3] = { globPlaneX[0] * dx, globPlaneX[1] * dx, globPlaneX[2] * dx };

	float yaxis[3] = { globPlaneY[0] * dy, globPlaneY[1] * dy, globPlaneY[2] * dy };

	float offset[3] = { globPlaneOrigin[0] - origin[0], globPlaneOrigin[1] - origin[1], globPlaneOrigin[2] - origin[2] };

	//set the size of texture
	albaGPUOGL* gpuProvider = glo_Textures[pos].m_pGPUProvider;
	gpuProvider->SetTarget(xs, ys, xs*dx, ys*dy);


	wxString szErr;
	if (!gpuProvider->BeginExecute(&szErr))
	{
		return; //terminate process
	}
	else
	{
			//set parameters
		//NB. this cannot be done in preprocessing
		gpuProvider->SetParameter("pl_x", xaxis);
		gpuProvider->SetParameter("pl_y", yaxis);
		gpuProvider->SetParameter("pl_ofs", offset);
		gpuProvider->SetParameter("dimension", glo_Textures[pos].m_DataDims);

		//bind our texture is in the first texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, m_TextureId);

		gpuProvider->SetParameter("voxels", (int)0);

		gpuProvider->InitializeComputation();
		gpuProvider->RunComputation();
		gpuProvider->FinalizeComputation();
		gpuProvider->GetResult(output, numComp > 1);
		gpuProvider->EndExecute();
	}
}

//----------------------------------------------------------------------------
void albaGPU3DTextureProviderHelper::CreateImage(vtkImageData *outputObject, float *globPlaneX, float *globPlaneY, float *globPlaneZ, double *globPlaneOrigin)
{
	vtkDataArray* pScalars = outputObject->GetPointData()->GetScalars();
	void *outputPointer = pScalars->GetVoidPointer(0);

	switch (pScalars->GetDataType())
	{
		case VTK_CHAR:
			CreateImage((char*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_UNSIGNED_CHAR:
			CreateImage((unsigned char*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_SHORT:
			CreateImage((short*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_UNSIGNED_SHORT:
			CreateImage((unsigned short*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_INT:
			CreateImage((int*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_UNSIGNED_INT:
			CreateImage((unsigned int*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_FLOAT:
			CreateImage((float*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		case VTK_DOUBLE:
			CreateImage((double*)outputPointer, outputObject, globPlaneX, globPlaneY, globPlaneZ, globPlaneOrigin);
			break;
		default:
			return;
	}
}
