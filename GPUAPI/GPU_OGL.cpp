/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: GPU_OGL
 Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk), Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifdef _WIN32
#define _WINSOCKAPI_

#endif // _WIN32
#include "GPU_OGL.h"
#include "albaDefines.h"
#include "albaDecl.h"

/*static*/bool albaGPUOGL::glo_bGPUOGLSupported = false;
/*static*/bool albaGPUOGL::glo_bGPUOGLInitialized = false;

albaGPUOGL *glo_GPUOGL=NULL;
unsigned int glo_RegCounter = 0;

#ifdef _WIN32
#ifdef _DEBUG
// Define targa header.
#pragma pack(1)
typedef struct
{
  GLbyte	identsize;              // Size of ID field that follows header (0)
  GLbyte	colorMapType;           // 0 = None, 1 = paletted
  GLbyte	imageType;              // 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle
  unsigned short	colorMapStart;          // First colour map entry
  unsigned short	colorMapLength;         // Number of colors
  unsigned char 	colorMapBits;   // bits per palette entry
  unsigned short	xstart;                 // image x origin
  unsigned short	ystart;                 // image y origin
  unsigned short	width;                  // width in pixels
  unsigned short	height;                 // height in pixels
  GLbyte	bits;                   // bits per pixel (8 16, 24, 32)
  GLbyte	descriptor;             // image descriptor
} TGAHEADER;
#pragma pack(8)



////////////////////////////////////////////////////////////////////
// Capture the current viewport and save it as a targa file.
// Be sure and call SwapBuffers for double buffered contexts or
// glFinish for single buffered contexts before calling this function.
// Returns 0 if an error occurs, or 1 on success.
GLint gltWriteTGA(const char *szFileName, GLenum gltyp)
{
  FILE *pFile;                // File pointer
  TGAHEADER tgaHeader;		// TGA file header
  unsigned long lImageSize;   // Size in bytes of image
  GLbyte	*pBits = NULL;      // Pointer to bits
  GLint iViewport[4];         // Viewport in pixels  

  // Get the viewport dimensions
  glGetIntegerv(GL_VIEWPORT, iViewport);

  // How big is the image going to be (targas are tightly packed)
  lImageSize = iViewport[2] * 3 * iViewport[3];	

  // Allocate block. If this doesn't work, go home
  pBits = (GLbyte *)malloc(lImageSize);
  if(pBits == NULL)
    return 0;

  // Read bits from color buffer
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);

  // Initialize the Targa header
  tgaHeader.identsize = 0;
  tgaHeader.colorMapType = 0;
  tgaHeader.imageType = 2;
  tgaHeader.colorMapStart = 0;
  tgaHeader.colorMapLength = 0;
  tgaHeader.colorMapBits = 0;
  tgaHeader.xstart = 0;
  tgaHeader.ystart = 0;
  tgaHeader.width = iViewport[2];
  tgaHeader.height = iViewport[3];
  tgaHeader.bits = 24;
  tgaHeader.descriptor = 0;

#pragma warning(push)
#pragma warning(disable: 4996)
  // Attempt to open the file
  pFile = albaTryOpenFile(szFileName, "wb");
  if(pFile == NULL)
  {
    free(pBits);    // Free buffer and return error
    return 0;
  }

#pragma warning(pop)

  // Write the header
  fwrite(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

  // Write the image data
  fwrite(pBits, lImageSize, 1, pFile);

  // Free temporary buffer and close the file
  free(pBits);    
  fclose(pFile);

  // Success!
  return 1;
}
#endif // _DEBUG
#endif // _WIN32





//----------------------------------------------------------------------------
albaGPUOGL * albaGPUOGL::BindGPUOGL()
{
	if (glo_GPUOGL == NULL)
		glo_GPUOGL = new albaGPUOGL();
	glo_RegCounter++;
	return glo_GPUOGL;
}

//----------------------------------------------------------------------------
void albaGPUOGL::ReleaseGPUOGL()
{
	glo_RegCounter--;
	assert(glo_RegCounter >= 0);
	if (glo_RegCounter == 0)
		cppDEL(glo_GPUOGL);
}

//ctor
albaGPUOGL::albaGPUOGL()
{
#ifdef _WIN32
  m_vs_ps[0] = m_vs_ps[1] = NULL;
  m_progObj = NULL;
  m_pFBO = NULL;
  m_bValidated = false;
  m_TextureId = 0;

  m_hPrevRWndGL = m_hRWndGL = NULL;
  m_hPrevRWndDC = m_hRWndHDC = NULL;  
  m_hRWnd = NULL; 
  m_nGPUGLContextRef = 0;
  m_bUnregWndClass = false;

  if (!glo_bGPUOGLInitialized)
  {
    glo_bGPUOGLSupported = CreateRenderingWindow(NULL);
    if (glo_bGPUOGLSupported)
    {
      //activate our rendering context
      albaGPUOGLContext context(this);

      glo_bGPUOGLSupported = glewInit() == GLEW_OK;
      if (glo_bGPUOGLSupported)
      {
        glo_bGPUOGLSupported = 
          IsExtSupported("GL_ARB_vertex_shader") &&
          IsExtSupported("GL_ARB_fragment_shader") &&
          IsExtSupported("GL_ARB_shader_objects") &&
          IsExtSupported("GL_ARB_shading_language_100") &&
          IsExtSupported("GL_EXT_framebuffer_object");
      }
    }

    glo_bGPUOGLInitialized = true;
  }
#endif

  m_TargetWidth = 256; m_TargetHeight = 256;
  m_TargetSizeCx = 1.0f; m_TargetSizeCy = 1.0f;
}

albaGPUOGL::~albaGPUOGL()
{
  DestroyShaders();
#ifdef _WIN32
  DestroyRenderingContext();
#endif // _WIN32
}

#ifdef _WIN32
//------------------------------------------------------------------------
// Results true if the given OpenGL extension is supported
bool albaGPUOGL::IsExtSupported(const char* extension)
//------------------------------------------------------------------------
{
  albaGPUOGLContext context(this);
  return glewGetExtension(extension) == GL_TRUE;
}

//------------------------------------------------------------------------
//Creates OpenGL rendering context. 
//Called from CreateShaders, returns false, if the context could not be created
bool albaGPUOGL::CreateRenderingContext(wxString* err)
//------------------------------------------------------------------------
{
  //create OpenGL rendering window
  if (!CreateRenderingWindow(err))
    return false;
  
  //activate our rendering context
  albaGPUOGLContext context(this);
  if (m_TextureId == 0)
    glGenTextures(1, &m_TextureId);

  if (m_pFBO == NULL)
    m_pFBO = new FramebufferObject();

  return true;
}

//------------------------------------------------------------------------
//Destroys the rendering context
void albaGPUOGL::DestroyRenderingContext()
//------------------------------------------------------------------------
{
  { //NB. this block here is required!
    //activate our rendering context
    albaGPUOGLContext context(this);

    if (m_TextureId != 0)
      glDeleteTextures(1, &m_TextureId);

    if (m_pFBO != NULL)
    {
      //unbind every FBO and return rendering to original way
      FramebufferObject::Disable();
      delete m_pFBO;
      m_pFBO = NULL;
    }
  }

  DestroyRenderingWindow();
}
#endif // _WIN32

//------------------------------------------------------------------------
//Initializes simple vertex and fragment shader 
//if a shader is not specified, the parameter vs or ps must be NULL
//returns false, if an error occurs and the description of the error in err parameter
bool albaGPUOGL::CreateShaders(const char* vs, const char* ps, wxString* err)
//------------------------------------------------------------------------
{
#ifndef _WIN32
  if (err != NULL)
    *err = "GPU OGL is not supported on the current OS platform";
  return false;
#else
  //we have to create rendering context (if it does not already exists)
  if (!CreateRenderingContext(err))
    return false;  

  albaGPUOGLContext myContext(this);  
  DestroyShaders();  //if we have previously created shaders, remove them first

  //vertex shader
  if (vs != NULL)    
  {
    m_vs_ps[0] = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    glShaderSourceARB(m_vs_ps[0], 1, (const GLcharARB**)&vs, NULL);
  }

  //fragment shader
  if (ps != NULL)
  {
    m_vs_ps[1] = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB(m_vs_ps[1], 1, (const GLcharARB**)&ps, NULL);
  }
    
  for (int i = 0; i < 2; i++)
  {
    // Compile shader and check for any errors
    if (m_vs_ps[i] == 0)
      continue; //missing shader

    GLint success;
    glCompileShaderARB(m_vs_ps[i]);
    glGetObjectParameterivARB(m_vs_ps[i], GL_OBJECT_COMPILE_STATUS_ARB, &success);
    if (!success)
    {
      if (err != NULL)
      {  
				err->Alloc(MAX_PATH + 1);
        glGetInfoLogARB(m_vs_ps[i], MAX_PATH, NULL, err->char_str());
      }

      return false;
    }
  }

  // Create program object, attach both shaders, then link
  m_progObj = glCreateProgramObjectARB();  
  if (m_vs_ps[0] != 0)
    glAttachObjectARB(m_progObj, m_vs_ps[0]);
  if (m_vs_ps[1] != 0)
    glAttachObjectARB(m_progObj, m_vs_ps[1]);
 
  GLint success;
  glLinkProgramARB(m_progObj);
  glGetObjectParameterivARB(m_progObj, GL_OBJECT_LINK_STATUS_ARB, &success);
  if (!success)
  {
    if (err != NULL)
    {        
			err->Alloc(MAX_PATH + 1);
      glGetInfoLogARB(m_progObj, MAX_PATH, NULL, err->char_str());
    }

    return false;
  }

  m_bValidated = false;
  return true;
#endif
}

//------------------------------------------------------------------------
// Releases OpenGL resources allocated for shaders by CreateShaders
void albaGPUOGL::DestroyShaders()
//------------------------------------------------------------------------
{
#ifdef _WIN32
  //activate our rendering context
  albaGPUOGLContext myContext(this);  

  for (int i = 0; i < 2; i++)
  {
    if (m_vs_ps[i] != 0) 
    {
      glDeleteObjectARB(m_vs_ps[i]);
      m_vs_ps[i] = 0;
    }
  }

  if (m_progObj != 0)
  {
    glDeleteObjectARB(m_progObj);
    m_progObj = 0;
  }
#endif // _WIN32
}

//------------------------------------------------------------------------
//Loads shading program into hardware pipeline 
//Every successive rendering will go through custom program
//Returns false, if the program could not be loaded. 
//------------------------------------------------------------------------
bool albaGPUOGL::BeginExecute(wxString* err)
{
#ifndef _WIN32
  return false;
#else
  //activate our rendering context
  albaGPUOGLContext myContext(this);  
  
  //load shader into hardware pipeline    
  glUseProgramObjectARB(m_progObj);

  if (!m_bValidated)
  {
    m_bValidated = true;
    GLint success;
		
    glValidateProgramARB(m_progObj);
    glGetObjectParameterivARB(m_progObj, GL_OBJECT_VALIDATE_STATUS_ARB, &success);
    if (!success)
    {
      if (err != NULL)
      { 
				err->Alloc(MAX_PATH + 1);
        glGetInfoLogARB(m_progObj, MAX_PATH, NULL, err->char_str());
      }

      EndExecute();
      return false;
    }
  }

  //resize window to fit the requested target
  RECT rc, rw;
  GetClientRect(m_hRWnd, &rc);
  GetWindowRect(m_hRWnd, &rw);
  rw.right = m_TargetWidth + rw.right - rc.right;
  rw.bottom = m_TargetHeight + rw.bottom - rc.bottom;
  MoveWindow(m_hRWnd, 0, 0, rw.right, rw.bottom, false);

  //bind the texture into which we will render
  glBindTexture(GL_TEXTURE_2D, m_TextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);      

  //BES: some problems may arise according to Baoquan because of internal format (GL_RGBA8) used in combination with GL_FLOAT
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, m_TargetWidth, m_TargetHeight, 0, GL_RGBA, GL_FLOAT, 0);

  if (glGetError() != GL_NO_ERROR)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_TargetWidth, m_TargetHeight, 0, GL_RGBA, GL_FLOAT, 0);
  m_pFBO->Bind(); // Bind framebuffer object.

  // Attach texture to framebuffer color buffer
  m_pFBO->AttachTexture(GL_TEXTURE_2D, m_TextureId, GL_COLOR_ATTACHMENT0_EXT);

  // Validate the FBO after attaching textures and render buffers
  m_pFBO->IsValid();

  glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);			 // Draw into the first texture  


  m_nGPUGLContextRef++;   //remain in the context so the user commands goes to our rendering context  
  return true;
#endif
}

//------------------------------------------------------------------------
// Unloads the current shading program
void albaGPUOGL::EndExecute()
//------------------------------------------------------------------------
{
#ifdef _WIN32
  albaGPUOGLContext myContext(this);
  m_nGPUGLContextRef--;   //quit the context (see BeginExecute)
#endif
}

//------------------------------------------------------------------------
//Sets program parameter 
//Must be called after BeginExecute
void albaGPUOGL::SetParameter(const char* paramname, float fvalue)
//------------------------------------------------------------------------
{
#ifdef _WIN32
  albaGPUOGLContext myContext(this);

  GLint iPos = glGetUniformLocationARB(m_progObj, paramname);
  if (iPos >= 0)
    glUniform1fARB(iPos, (GLfloat)fvalue);
#endif
}

//------------------------------------------------------------------------
//Sets program parameter 
//Must be called after BeginExecute
void albaGPUOGL::SetParameter(const char* paramname, int ivalue)
//------------------------------------------------------------------------
{
#ifdef _WIN32
  albaGPUOGLContext myContext(this);

  GLint iPos = glGetUniformLocationARB(m_progObj, paramname);
  if (iPos >= 0)
    glUniform1iARB(iPos, (GLint)ivalue);
#endif
}

//------------------------------------------------------------------------
//Sets program parameter 
//Must be called after BeginExecute
void albaGPUOGL::SetParameter(const char* paramname, float fvector[3])
//------------------------------------------------------------------------
{
#ifdef _WIN32
  albaGPUOGLContext myContext(this);

  GLint iPos = glGetUniformLocationARB(m_progObj, paramname);
  if (iPos >= 0)
    glUniform3fARB(iPos, (GLfloat)fvector[0], (GLfloat)fvector[1], (GLfloat)fvector[2]);
#endif
}

//------------------------------------------------------------------------
//Sets program parameter 
//Must be called after BeginExecute
void albaGPUOGL::SetParameter(const char* paramname, int ivector[3])
//------------------------------------------------------------------------
{
#ifdef _WIN32
  albaGPUOGLContext myContext(this);

  GLint iPos = glGetUniformLocationARB(m_progObj, paramname);
  if (iPos >= 0)
    glUniform3iARB(iPos, (GLint)ivector[0], (GLint)ivector[1], (GLint)ivector[2]);
#endif
}

//------------------------------------------------------------------------
//Initializes computation
//Initialize camera, viewport, etc.
/*virtual*/ void albaGPUOGL::InitializeComputation()
//------------------------------------------------------------------------
{  
#ifdef _WIN32  
  glViewport(0, 0, m_TargetWidth, m_TargetHeight);

  glMatrixMode(GL_PROJECTION);  
  glLoadIdentity();
  //gluOrtho2D(0.0, m_TargetSizeCx, 0.0, m_TargetSizeCy);  
  gluOrtho2D(-1, 1, -1, 1);

  glMatrixMode(GL_MODELVIEW);  
  glLoadIdentity();
#endif
}

//------------------------------------------------------------------------
//Sends geometry into the hardware pipeline to run shaders
/*virtual*/ void albaGPUOGL::RunComputation()
//------------------------------------------------------------------------
{
#ifdef _WIN32  
  //reset all values to 0
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);  

  //set color to white
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS);
    //CW oriented    
    glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.5f);
    glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.5f);
    glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.5f);
    glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.5f);  
  glEnd();  
#endif
}

//------------------------------------------------------------------------
//Finalizes computation
//It resets camera, viewport, etc. to original state
/*virtual*/ void albaGPUOGL::FinalizeComputation()
//------------------------------------------------------------------------
{  
#if _WIN32
//  SwapBuffers(m_hRWndHDC);
#endif
}

//------------------------------------------------------------------------
//Runs the shading program using the internal execution pipeline
//It calls BeginExecution, InitializeComputation, RunComputation, 
//FinalizeComputation and EndExecution  
//Returns false, if the program could not be executed.
bool albaGPUOGL::ExecuteProgram(wxString* err)
//------------------------------------------------------------------------
{
  //switch to our rendering context
  if (!BeginExecute(err))
    return false;

  InitializeComputation();
  RunComputation();
  FinalizeComputation();

  EndExecute();   
  //now we left our rendering context
  return true;
}

#ifdef _WIN32
//------------------------------------------------------------------------
//Copies the result from the framebuffer (target) into the given buffer
//The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
//bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget
void albaGPUOGL::GetResult(void* pBuffer, GLenum data_type, bool bGetVector)
//------------------------------------------------------------------------
{
  //switch to our context
  albaGPUOGLContext context(this);

  glPushAttrib(GL_ALL_ATTRIB_BITS);  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //we will read from the texture
  glBindTexture(GL_TEXTURE_2D, m_TextureId);
  glGetTexImage(GL_TEXTURE_2D, 0, 
     (bGetVector ? GL_RGB : GL_ALPHA), data_type, pBuffer);

#ifdef _DEBUG
  GLenum err = glGetError();
  assert(err == GL_NO_ERROR);

  //static int nCurImage = 0;
  //gltWriteTGA(albaString::Format("g:\\pokus_%02d_GPU.tga", nCurImage), 0);
  //if (++nCurImage == 100)
  //  nCurImage = 0;  //wrap
#endif // _DEBUG

  //restore pixel storei to original one
  glPixelStorei(GL_PACK_ALIGNMENT, 4);   
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  
  glPopAttrib();
}

#define RW_CLASSNAME "albaGPUOGL_RW"

//------------------------------------------------------------------------
//Creates OpenGL rendering window
//Returns false, if the OpenGL rendering window could not be created.
bool albaGPUOGL::CreateRenderingWindow(wxString* err)
//------------------------------------------------------------------------
{
  if (m_hRWnd == NULL)
  {
    //register new window class
    WNDCLASS	wc;
    ZeroMemory (&wc, sizeof (WNDCLASS));

    wc.style			    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc	  = DefWindowProc;
    wc.hInstance		  = GetModuleHandle(NULL);
		
		int len = MultiByteToWideChar(CP_ACP, 0, RW_CLASSNAME, -1, NULL, 0);
// 		LPCSTR myWideString = new CHAR[len];
// 		LPWSTR tmp;
// 		MultiByteToWideChar(CP_ACP, 0, RW_CLASSNAME, -1, myWideString, len);

    wc.lpszClassName	= RW_CLASSNAME;

    if (RegisterClass(&wc))
      m_bUnregWndClass = true;
    else
    {
      if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
      {        
        GetLastErrorText(err);
        return false;
      }
    }

    //create a new window
    m_hRWnd = CreateWindowA(RW_CLASSNAME, "", WS_OVERLAPPED, 0, 0,m_TargetWidth, m_TargetHeight, 
      NULL, NULL, wc.hInstance, 0);

    if (m_hRWnd == NULL)
    {
      GetLastErrorText(err);
      return false;
    }
  }

  if (m_hRWndHDC == NULL)
  {
    //create new window device context
    if (NULL == (m_hRWndHDC = GetDC(m_hRWnd)))
    {
      GetLastErrorText(err);
      return false;
    }

    //set pixel format
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory (&pfd, sizeof (pfd));

    pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion		= 1;
    pfd.dwFlags			= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |	PFD_DOUBLEBUFFER;
    pfd.iPixelType		= PFD_TYPE_RGBA;
    pfd.cColorBits		= 24;
    pfd.cStencilBits	= 8;
    pfd.cDepthBits		= 24;

    int format;
    if ((format = ChoosePixelFormat(m_hRWndHDC, &pfd)) == 0 ||
      !SetPixelFormat(m_hRWndHDC, format, &pfd))
    {
      GetLastErrorText(err);
      ReleaseDC(m_hRWnd, m_hRWndHDC);
      m_hRWndHDC = NULL;
      return false;
    }
  }

  if (m_hRWndGL == NULL)
  {
    //create new OpenGL rendering context    
    if (NULL == (m_hRWndGL = wglCreateContext(m_hRWndHDC)))
    {
      GetLastErrorText(err);
      return false;
    }    
  }

  return true;
}

//------------------------------------------------------------------------
// Destroys the rendering window
void albaGPUOGL::DestroyRenderingWindow()
//------------------------------------------------------------------------
{
  assert(m_nGPUGLContextRef == 0);
  if (m_hRWndGL != NULL)
  {   
    //wglMakeCurrent (NULL, NULL);   
    wglDeleteContext(m_hRWndGL);
    m_hRWndGL = NULL;
  }

  if (m_hRWndHDC != NULL)
  {
    ReleaseDC(m_hRWnd,m_hRWndHDC);
    m_hRWndHDC = NULL;
  }

  if (m_hRWnd != NULL)
  {
    DestroyWindow(m_hRWnd);
    m_hRWnd = NULL;
  }

  if (m_bUnregWndClass) 
  {
	/*	int len = MultiByteToWideChar(CP_ACP, 0, RW_CLASSNAME, -1, NULL, 0);
		wchar_t* myWideString = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, RW_CLASSNAME, -1, myWideString, len);
		*/
    UnregisterClass(RW_CLASSNAME, ::GetModuleHandle(NULL));
    m_bUnregWndClass = false;
  }
}


//------------------------------------------------------------------------
//Formats Windows LastError into err
void albaGPUOGL::GetLastErrorText(wxString* err)
//------------------------------------------------------------------------
{
  if (err != NULL)
  {
		LPSTR lpMsgBuf = nullptr; 
    DWORD dw = GetLastError(); 

    FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPSTR) &lpMsgBuf,
      0, NULL );

    err->Printf("%s", lpMsgBuf);
    LocalFree(lpMsgBuf);
  }
}

//------------------------------------------------------------------------
// Switch to our OpenGL context
albaGPUOGL::albaGPUOGLContext::albaGPUOGLContext(albaGPUOGL* pThis) : m_pThis(pThis)
//------------------------------------------------------------------------
{  
  if (pThis->m_nGPUGLContextRef == 0)
  {
    pThis->m_hPrevRWndGL = wglGetCurrentContext();
    pThis->m_hPrevRWndDC = wglGetCurrentDC();
    wglMakeCurrent(pThis->m_hRWndHDC, pThis->m_hRWndGL);
  }

  pThis->m_nGPUGLContextRef++;
}

//------------------------------------------------------------------------
//Restores the original OpenGL context
albaGPUOGL::albaGPUOGLContext::~albaGPUOGLContext()
//------------------------------------------------------------------------
{
  if ((--m_pThis->m_nGPUGLContextRef) == 0)
    wglMakeCurrent(m_pThis->m_hPrevRWndDC, m_pThis->m_hPrevRWndGL);
}

//------------------------------------------------------------------------
//Activates the OpenGL rendering context of this object
//Typically used in preprocessing to load input data 
//NB. DisableRenderingContext should be called asap
void albaGPUOGL::EnableRenderingContext()
//------------------------------------------------------------------------
{
  albaGPUOGLContext context(this);
  m_nGPUGLContextRef++;
}

//------------------------------------------------------------------------
//Deactivates the OpenGL rendering context of this object activated by EnableRenderingContext method.
void albaGPUOGL::DisableRenderingContext()
//------------------------------------------------------------------------
{
  albaGPUOGLContext context(this);
  m_nGPUGLContextRef--;
}
#endif // _WIN32