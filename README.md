# Open IDE
This is an open source IDE that you can hack apart, use as a foundation, or practice with.

This is not a glorious, production-grade IDE, I simply wrote this because I was bored.

### Known Issues
Some known issues that should get addressed:
* (`v0.3.0`) Syntax highlighting does not work on MacOS
* (`v0.1.0+`) File tree on MacOS is sorted ascending regardless of directory/file (directories should always show first)
  * This may just be a limitation with QTreeView on MacOS, but maybe can find some workarounds

# Building from Source
This is a Qt6 cmake project. Install the prerequisites and follow the build instructions below:
## Prerequisites
Install the following dependencies based on your OS. Be sure you have a C11 and C++17 compatible compiler (gcc, clang, msvc, etc.)
### Linux
```
sudo apt-get install cmake qtcreator build-essential libgl1-mesa-dev qt6-base-dev qt6-tools-dev-tools
```

### MacOS
```
brew install cmake qt@6
```

### Windows
Recommended to install the QtCreator on windows which will also install other dependencies for you: https://doc.qt.io/qtcreator/creator-how-to-install.html

## Cloning and Building
**IMPORTANT**: When cloning the repository, use the `--recursive` flag to include Tree-sitter submodules:
```bash
git clone --recursive https://github.com/Kiyoshika/openIDE.git
```

If you've already cloned without `--recursive`, initialize the submodules:
```bash
git submodule update --init --recursive
```

### Building with QtCreator
If building with QtCreator, just `File > Open File or Project` and select the root `CMakeLists.txt`. Everything should configure automaticaly after importing. 

### Building with CMake Directly
If you want to build manually with CMake, it's fairly straightforward. Ensure you have the above Qt6 dependencies installed, then:
1. From project root: `mkdir build && cd build`
2. `cmake ..`
3. `make`
4. Start the app: `./build/src/openIDE`

## Releases
Check out the github [releases](https://github.com/Kiyoshika/openIDE/tags) for snapshot builds

**WARNING**: The `main` branch may be unstable as it is the active development/experimental branch. Stick to releases for the "stable"-ish builds.

## Preview 
Below is a preview of the `0.3.0` build on `2025 December 28` (dark theme)

<img width="1539" height="884" alt="image" src="https://github.com/user-attachments/assets/5677f477-9568-49d5-a6bc-159b55e5225a" />

## Wishlist
These are the features that I eventually want to implement (in no particular order)
* Right-click context menus
    * CodeEditor - view definitions/implementation, refactor, etc. (need LSP first)
* Vim motions
* Adding build/run configurations (and saving/exporting them)
* Support for custom themes (+ importing/exporting themes)
* Adjust scale of UI somehow (configurable in settings)
* File diffing
* Git integration
* Code traversal (goto definition/implementation, etc.)
* Support for language server protocols, error highlighting/syntax checking, etc.
* Setting up breakpoints and debugging
* Add fuzzy finder to search files or content within files
* Live file reads - as of now, if a change to a file is made externally, it must be closed and re-opened. But, like other editors, I'd like to have a live view of the file to reflect external changes immediately (need some thought on how to do this properly and handling conflicts such as editing a file while a change happened externally) - we can also potentially poll for file metadata and checking if it has been updated since we've opened it and display a message if we want to load the changes (if the file has unsaved changes) or if the file is clean, automatically read the external changes]
* Code formatting
* Code folding/unfolding (fold/unfold current method or all methods in file, etc.)

## Roadmap
Below is the rough roadmap for the next release(s) targeting major features I want to accomplish

### v0.4.0 Roadmap
The goal of this release is to add the remaining piece to be able to actually write software entirely within openIDE. With a functioning editor, file traversal and integrated terminal, this fully completes the bare minimum development loop.
* Integrated terminal
   * This alone might be pretty complex, quick google search doesn't seem to be supported out of the box so will have to be a custom implementation
* Run configurations
   * When creating/opening a project, it will create a `.openide/` directory to store run configurations (this can be gitignored) and other settings later
   * There will be a dropdown on the right-hand side for the current selected configuration (default `<none>`) and an option to create a new configuration
   * Through the `Build` menu (to be created) you can:
      * Edit configurations (run, build, test, eventually debug)
      * Run/Build/Test (which will use above-mentioned configurations) - these will open 
   * The configurations themselves are just wrappers around commands that you specify (e.g., `cmake .. && make` within a specified build directory)
   * For some languages, defaults will be provided to reduce manual work (e.g., cmake for C/C++, gradle/maven for Java, pyenv/pip for Python, etc.)
   * The required build tools (e.g., cmake, gradle, etc.) will NOT be packaged and assumed user has them installed - however, if we write an installer for openIDE, perhaps we could provide the option to isntall tools if needed

### v0.5.0 Roadmap
This release will focus entirely on language server support to be able to traverse through code, highlight syntax errors and provide other useful development information
* Add a new menu `Settings > Language Server` to select a language server to use for the specified language(s) - this will assume the user has them installed and will not install them for you (yet)
  * Probably need to also specify a command on how to invoke/start the language server which is triggered when opening a file type corresponding to the given LSP
* If LSP is configured when opening relevant file type, need to way to invoke the command, receive information, parse it and display relevant information on the code editor as well as supporting events to click on method names, view references, goto definitions, etc.
* Possibly throw in the code formatting in this release `Settings > Code Formatting` which will have a similar setup to the LSP (external tool + applies to specific files opened), add a context menu item "Format Code". Also add the ability in settings to provide additional arguments to the formatter of choice

### v0.6.0 Roadmap
This release will focus entirely on debugging.
* Add debuggers to the configurations introduced in `v0.4.0`
* Set regular/conditional breakpoints (or maybe regular breakpoints to get started) in the editor
* Be able to step through code, run expressions, view variables/stack information
 

## Third-Party Libraries

### Tree-sitter
This project uses [Tree-sitter](https://github.com/tree-sitter/tree-sitter) for syntax highlighting and parsing.

**License:** MIT License  
**Copyright:** © 2018-2024 Max Brunsfeld

Tree-sitter is an incremental parsing system for programming tools. We use it to provide accurate, efficient syntax highlighting across 20+ programming languages including C, C++, Python, Java, JavaScript, TypeScript, Go, Rust, C#, Ruby, PHP, Swift, Kotlin, HTML, CSS, SQL, Shell, Markdown, JSON, XML, and YAML.

Full license text available at: `external/tree-sitter/LICENSE`

## License
This Qt project is licensed under LPGLv3
```
GNU LESSER GENERAL PUBLIC LICENSE
                       Version 3, 29 June 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <http://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

  This version of the GNU Lesser General Public License incorporates
the terms and conditions of version 3 of the GNU General Public
License, supplemented by the additional permissions listed below.

  0. Additional Definitions.

  As used herein, "this License" refers to version 3 of the GNU Lesser
General Public License, and the "GNU GPL" refers to version 3 of the GNU
General Public License.

  "The Library" refers to a covered work governed by this License,
other than an Application or a Combined Work as defined below.

  An "Application" is any work that makes use of an interface provided
by the Library, but which is not otherwise based on the Library.
Defining a subclass of a class defined by the Library is deemed a mode
of using an interface provided by the Library.

  A "Combined Work" is a work produced by combining or linking an
Application with the Library.  The particular version of the Library
with which the Combined Work was made is also called the "Linked
Version".

  The "Minimal Corresponding Source" for a Combined Work means the
Corresponding Source for the Combined Work, excluding any source code
for portions of the Combined Work that, considered in isolation, are
based on the Application, and not on the Linked Version.

  The "Corresponding Application Code" for a Combined Work means the
object code and/or source code for the Application, including any data
and utility programs needed for reproducing the Combined Work from the
Application, but excluding the System Libraries of the Combined Work.

  1. Exception to Section 3 of the GNU GPL.

  You may convey a covered work under sections 3 and 4 of this License
without being bound by section 3 of the GNU GPL.

  2. Conveying Modified Versions.

  If you modify a copy of the Library, and, in your modifications, a
facility refers to a function or data to be supplied by an Application
that uses the facility (other than as an argument passed when the
facility is invoked), then you may convey a copy of the modified
version:

   a) under this License, provided that you make a good faith effort to
   ensure that, in the event an Application does not supply the
   function or data, the facility still operates, and performs
   whatever part of its purpose remains meaningful, or

   b) under the GNU GPL, with none of the additional permissions of
   this License applicable to that copy.

  3. Object Code Incorporating Material from Library Header Files.

  The object code form of an Application may incorporate material from
a header file that is part of the Library.  You may convey such object
code under terms of your choice, provided that, if the incorporated
material is not limited to numerical parameters, data structure
layouts and accessors, or small macros, inline functions and templates
(ten or fewer lines in length), you do both of the following:

   a) Give prominent notice with each copy of the object code that the
   Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the object code with a copy of the GNU GPL and this license
   document.

  4. Combined Works.

  You may convey a Combined Work under terms of your choice that,
taken together, effectively do not restrict modification of the
portions of the Library contained in the Combined Work and reverse
engineering for debugging such modifications, if you also do each of
the following:

   a) Give prominent notice with each copy of the Combined Work that
   the Library is used in it and that the Library and its use are
   covered by this License.

   b) Accompany the Combined Work with a copy of the GNU GPL and this license
   document.

   c) For a Combined Work that displays copyright notices during
   execution, include the copyright notice for the Library among
   these notices, as well as a reference directing the user to the
   copies of the GNU GPL and this license document.

   d) Do one of the following:

       0) Convey the Minimal Corresponding Source under the terms of this
       License, and the Corresponding Application Code in a form
       suitable for, and under terms that permit, the user to
       recombine or relink the Application with a modified version of
       the Linked Version to produce a modified Combined Work, in the
       manner specified by section 6 of the GNU GPL for conveying
       Corresponding Source.

       1) Use a suitable shared library mechanism for linking with the
       Library.  A suitable mechanism is one that (a) uses at run time
       a copy of the Library already present on the user's computer
       system, and (b) will operate properly with a modified version
       of the Library that is interface-compatible with the Linked
       Version.

   e) Provide Installation Information, but only if you would otherwise
   be required to provide such information under section 6 of the
   GNU GPL, and only to the extent that such information is
   necessary to install and execute a modified version of the
   Combined Work produced by recombining or relinking the
   Application with a modified version of the Linked Version. (If
   you use option 4d0, the Installation Information must accompany
   the Minimal Corresponding Source and Corresponding Application
   Code. If you use option 4d1, you must provide the Installation
   Information in the manner specified by section 6 of the GNU GPL
   for conveying Corresponding Source.)

  5. Combined Libraries.

  You may place library facilities that are a work based on the
Library side by side in a single library together with other library
facilities that are not Applications and are not covered by this
License, and convey such a combined library under terms of your
choice, if you do both of the following:

   a) Accompany the combined library with a copy of the same work based
   on the Library, uncombined with any other library facilities,
   conveyed under the terms of this License.

   b) Give prominent notice with the combined library that part of it
   is a work based on the Library, and explaining where to find the
   accompanying uncombined form of the same work.

  6. Revised Versions of the GNU Lesser General Public License.

  The Free Software Foundation may publish revised and/or new versions
of the GNU Lesser General Public License from time to time. Such new
versions will be similar in spirit to the present version, but may
differ in detail to address new problems or concerns.

  Each version is given a distinguishing version number. If the
Library as you received it specifies that a certain numbered version
of the GNU Lesser General Public License "or any later version"
applies to it, you have the option of following the terms and
conditions either of that published version or of any later version
published by the Free Software Foundation. If the Library as you
received it does not specify a version number of the GNU Lesser
General Public License, you may choose any version of the GNU Lesser
General Public License ever published by the Free Software Foundation.

  If the Library as you received it specifies that a proxy can decide
whether future versions of the GNU Lesser General Public License shall
apply, that proxy's public statement of acceptance of any version is
permanent authorization for you to choose that version for the
Library.
```
