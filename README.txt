MAF Multimod Application Framework
Instruction for building:

WINDOWS:
MAF currently compiles only under Windows and it's building has been tested with the following configuration:
- CMake 1.6.7 from kitware (see later)
- Microsoft Visual Studio 6 (SP5).

Setup and Build:
   1. Place somewhere the source dir (typically e.g. c:\MAF\mmMSW
   2. Download Cmake 1.6.7 from the kitware web site:  http://www.cmake.org/HTML/Download.html
   3. Run CMake and configure the project, by specifying the source tree and where to place the compiled binaries, usually we place the Build directory just behind the sources one: e.g. c:\MAF\mmMSW for the sources and c:\MAF\Build for the binaries. Configuring will take some steps: simply continue to press the "Configure" button until all fields become gray, and then press "OK" to generate the project file.
   4. Using Visual Studio 6 (I'm sorry bu VS7 or later won't work, probably we'll have to upgrade the wxWindows library to make it working, since MAF projects imports wxWindows' ".dsp" files) and open the "maf.dsw" workspace file in the build directory. Note: it's a very big project, since it includes VTK, ITK and wxWindows, it will take some time to parse all classes (currently, we only support the full source distribution).
   5. You'll found a subproject called "Datamanager": that's the main multimod application. Compile it (I suggest you to verify the "Build->Set Active Configuration" is set to "Datamanager - Win32 Debug", since sometimes the compiler switches automatically to "RelWithDebInfo")

Some extra hints:
To speedup the compilation it is suggested to dispable the "VTK_USE_ANSI_STDLIB" option from in the CMake panel, this will increase compile time up to 4x. Currently defualt is ANSI_STD on, but we still support non-ANSI compilation to speed up the compile time with VC6.

Once you have compiled the whole project, you can go to a subdirectory in the Build directory and open a subproject (e.g. the Datamanager.dsp one ) to speed up your load and compile times. By opening the ".dsp" file, visual studio will automatically generate a corresponding .dsw one. I suggest you to add the following subprojects to the workspace: "mafCore.dsp" "mafLibrary.dsp" "mflCore.dsp" "vtkSVTK.dsp" (you have to search for them in the Buid directory) and set the Datamanager dependencies from the Project->Dependencies menu).


UNIX:
UNIX BUILD IS CURRENTLY NOT WORKING, IT SHOULD BE FIXED WITHIN NEXT 6 MONTHS.
  1.  run "ccmake" from the MAF root directory
  2.  adjust the verious settings, in particular (settings
      to be change to default in the future):
      enable build of examples

      run configure (press c)
      run generate (press g)

  3.  enter the "lal" subdir in the binary directory and launch:
      sh Configure_WXGTK.sh
      sh Build_WXGTK.sh

  4.  go back to the MAF root directory and run
      make

Notes for Unix:
Make program: on systems where gmake is available ccmake tends to use it. To verify which makefile is used by cmake press 't' from within the ccmake text form, to enable advanced settings, and look for the CMAKE_MAKE_PROGRAM variable.
