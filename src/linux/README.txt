Compilation Pre-requisites:
==========================
For compiling the GUI version, JAVA needs to be installed on the device, and JAVA_HOME environment variable needs to be set.
Refer JAVA installation instructions for details on how to set the environment variable.

Compilation Instructions:
========================
    make all    -    compiles everything
    make inode  -    Inode module
    make gbd    -    Group Descriptor module
    make mbr    -    MBR module
    make superblock -   Super Block module
    make clean  -   removes executables and object files

    After successful compilation the executables would be placed in the bin/
    directory.

To enable debug prints:
======================
    uncomment DEBUG_FLAGS = -DTRACE_WANTED in make.h

Generate Doxygen:
================
    Doxygen developer guid is currently provided only for 'inode' module.
See 'documentation.html' from 'inode/docs' directory for developer guide.
To generate new documentation, go to inode directory and run 'make doxy'.

Coding Guidelines:
=================
    Refer the document 'CodingStandards.docx' for rules and conventions to be
    followed for coding.

Running the code:
================
    User needs to be a superuser to run the executables placed in the bin directory.
