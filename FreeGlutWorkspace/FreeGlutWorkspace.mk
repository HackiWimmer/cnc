##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=FreeGlutWorkspace
ConfigurationName      :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/FreeGlutWorkspace
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=17/05/2019
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/msys64/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/msys64/mingw64/bin/g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG $(PreprocessorSwitch)_USE_WX_DUMMY_CANVAS 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="FreeGlutWorkspace.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui $(IncludeSwitch)C:\msys64\mingw64\x86_64-w64-mingw32\include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)opengl32 $(LibrarySwitch)glu32 $(LibrarySwitch)glew32 $(LibrarySwitch)freeglut 
ArLibs                 :=  "opengl32" "glu32" "glew32" "freeglut" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/msys64/mingw64/bin/ar.exe rcu
CXX      := C:/msys64/mingw64/bin/g++.exe
CC       := C:/msys64/mingw64/bin/gcc.exe
CXXFLAGS := -Wno-deprecated-declarations -Ofast -std=c++14 -std=c++11 -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -Ofast -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/msys64/mingw64/bin/as.exe


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) $(IntermediateDirectory)/Sample2.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Release"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLOpenGLPathBufferStore.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLOpenGLPathBufferStore.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(DependSuffix): ../CncControlerGui/3D/GLOpenGLPathBufferStore.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLOpenGLPathBufferStore.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLOpenGLPathBufferStore.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLOpenGLPathBufferStore.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLOpenGLPathBufferStore.cpp

$(IntermediateDirectory)/Sample2.cpp$(ObjectSuffix): Sample2.cpp $(IntermediateDirectory)/Sample2.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/FreeGlutWorkspace/Sample2.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Sample2.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Sample2.cpp$(DependSuffix): Sample2.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Sample2.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Sample2.cpp$(DependSuffix) -MM Sample2.cpp

$(IntermediateDirectory)/Sample2.cpp$(PreprocessSuffix): Sample2.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Sample2.cpp$(PreprocessSuffix) Sample2.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


