/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: GPU_OGL.h,v $ 
  Language: C++ 
  Date: $Date: 2008-12-12 11:00:02 $ 
  Version: $Revision: 1.1.2.1 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef GPU_OGL_h__
#define GPU_OGL_h__

#include <wx/string.h>


#ifdef _WIN32
//	#define GLEW_STATIC
	#include "glew/glew.h"    //must be included before gl.h and glu.h
	
//	#pragma message("Information: application must be linked with the static GLEW library")
#ifdef _DEBUG
//	#pragma message("Automatic linking with GLEW library: glew32sd.lib")	
//	#pragma comment( lib, "glew32sd.lib" ) 
#else
//	#pragma message("Automatic linking with GLEW library: glew32s.lib")	
//	#pragma comment( lib, "glew32s.lib" ) 
#endif

	#include "FBO/framebufferObject.h"	
#else
	#pragma message ("GPU_OGL is supported for WIN32 platform only");
#endif

class mafGPUOGL
{
protected:
  //singleton
  static bool m_bGPUOGLInitialized;
  static bool m_bGPUOGLSupported;

protected:
#ifdef _WIN32
  //smart class for context switch
  class mafGPUOGLContext
  {
  private:
    mafGPUOGL* m_pThis;
  public:
    /** Switch to our OpenGL context */
    mafGPUOGLContext(mafGPUOGL* pThis);

    /** Restores the original OpenGL context */
    ~mafGPUOGLContext();
  };

  friend class mafGPUOGLContext;


  HWND m_hRWnd;         //<rendering window
  HDC m_hRWndHDC;       //<Windows device context  
  HGLRC m_hRWndGL;      //<OpenGL rendering device context
  int m_nGPUGLContextRef;  //<larger than 0, if our OpenGL context is active

  HDC m_hPrevRWndDC;    //<previous Windows device context associated with the current thread
  HGLRC m_hPrevRWndGL;  //<previous OpenGL rendering device context associated with the current thread
  bool m_bUnregWndClass;//<true, if this object has registered window class

  GLhandleARB m_vs_ps[2];       //<vertex and fragment (pixel) shader
  GLhandleARB m_progObj;        //<program object with associated shaders  
  bool m_bValidated;            //<true if the current program was already validated

  FramebufferObject* m_pFBO;      //<Frame buffer object for rendering, see www.gpgpu.org
  unsigned int m_TextureId;     //<Texture into which we will render
  GLenum m_OriginalDrawbuf;     //<Identifier of original drawing buffer

#endif

  int m_TargetWidth;            //<the width of output texture in pixels
  int m_TargetHeight;           //<the height of output texture in pixels 
  float m_TargetSizeCx;         //<the width of output texture in mm (computation domain)
  float m_TargetSizeCy;         //<the height of output texture in mm (computation domain)
  
public:
  //ctor & dtor
  mafGPUOGL();
  ~mafGPUOGL();

public:
  /** Results true if OpenGL GPU is supported */
  inline static bool IsSupported() {
    if (!m_bGPUOGLInitialized) {
      mafGPUOGL init;   //this will perform initialization in the constructor
    }

    return m_bGPUOGLSupported;
  }

  /** Initializes simple vertex and fragment shader 
  if a shader is not specified, the parameter vs or ps must be NULL
  returns false, if an error occurs and the description of the error in err parameter */  
  bool CreateShaders(const char* vs, const char* ps, wxString* err = NULL);
  
  /** Releases OpenGL resources allocated for shaders by CreateShaders */
  void DestroyShaders();

  /** Runs the shading program using the internal execution pipeline
  It calls BeginExecution, InitializeComputation, RunComputation, 
  FinalizeComputation and EndExecution  
  Returns false, if the program could not be executed. */
  bool ExecuteProgram(wxString* err = NULL);

  /** Loads shading program into hardware pipeline 
  Every successive rendering will go through custom program 
  User calls BeginExecute and EndExecute, if additional or nonstandard
  computation is required, otherwise ExecuteProgram is better suitable
  Returns false, if the program could not be loaded. */
  virtual bool BeginExecute(wxString* err = NULL);

  /** Unloads the current shading program */
  virtual void EndExecute();

  /** Sets program parameter 
  Must be called after BeginExecute*/
  void SetParameter(const char* paramname, float fvalue);
  void SetParameter(const char* paramname, int ivalue);
  void SetParameter(const char* paramname, float fvector[3]);
  void SetParameter(const char* paramname, int ivector[3]);

  /** Set target texture size, i.e., sets computation domain
  width x height is number of pixels of texture, whilst ws x hs
  defines the domain covered by this texture (i.e., geometry)
  Must be called in prior to ExecuteProgram or BeginExecute*/
  inline void SetTarget(int width, int height, float ws, float hs) {
    m_TargetWidth = width; m_TargetHeight = height;
    m_TargetSizeCx = ws; m_TargetSizeCy = hs;
  }

  /** Initializes computation
  Initialize camera, viewport, etc. */
  virtual void InitializeComputation();

  /** Sends geometry into the hardware pipeline to run shaders */
  virtual void RunComputation();
  
  /** Finalizes computation
  It resets camera, viewport, etc. to original state */
  virtual void FinalizeComputation();

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(char* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_BYTE, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(unsigned char* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_UNSIGNED_BYTE, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(short* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_SHORT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(unsigned short* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_UNSIGNED_SHORT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(int* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_INT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(unsigned int* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_UNSIGNED_INT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(long* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_INT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(unsigned long* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_UNSIGNED_INT, bGetVector);
#endif
  }

  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  inline void GetResult(float* pBuffer, bool bGetVector = true) {
#ifdef _WIN32
    GetResult(pBuffer, GL_FLOAT, bGetVector);
#endif
  }

#ifdef _WIN32
  /** Copies the result from the framebuffer (target) into the given buffer
  The buffer must be capable to hold W*H pixels, i.e., W*H bytes if 
  bGetVector is false, otherwise 3*W*H - W and H are set in SetTarget*/
  void GetResult(void* pBuffer, GLenum data_type, bool bGetVector);
#endif

  /** Activates the OpenGL rendering context of this object
  Typically used in preprocessing to load input data 
  NB. DisableRenderingContext should be called asap*/
  void EnableRenderingContext();

  /** Deactivates the OpenGL rendering context of this object activated by EnableRenderingContext method. */
  void DisableRenderingContext();

protected:
  /** Results true if the given OpenGL extension is supported */
  bool IsExtSupported(const char* extension);  

  /** Creates OpenGL rendering context. 
  Called from CreateShaders, returns false, if the context could not be created */
  bool CreateRenderingContext(wxString* err = NULL);

  /** Destroys the rendering context */
  void DestroyRenderingContext();

  /** Creates OpenGL rendering window
  Returns false, if the OpenGL rendering window could not be created. */
  bool CreateRenderingWindow(wxString* err = NULL);

  /** Destroys the rendering window */
  void DestroyRenderingWindow();

  /** Formats Windows LastError into err */
  void GetLastErrorText(wxString* err);
};

#endif // GPU_OGL_h__