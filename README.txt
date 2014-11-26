MAF Multimod Application Framework.

[Copyright to be added]

[Some notes to be added]

On Microsoft Vista OS:
it is no possible to run patch.exe from command line unless you are under the administrator account 
(you have to run from the administrator account, belonging to "administrators" group doesn't suffice). 
In fact cmakelists files for vtk and itk should be patched, and under vista the any program that is 
named patch or install or setup requires user access control (which is an interactive control).  
To run without problems there are two alternatives: or to run CMake and Visual Studio as Administrators 
(even if you have admin privileges) or to disable this security features in Vista.  
We suggest the first option.  



