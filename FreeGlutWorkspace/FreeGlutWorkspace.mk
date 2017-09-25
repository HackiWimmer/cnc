##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=FreeGlutWorkspace
ConfigurationName      :=Debug
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/FreeGlutWorkspace
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=30/09/2017
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
SharedObjectLinkerName :=C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe -shared -fPIC
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
Preprocessors          :=$(PreprocessorSwitch)_USE_WX_DUMMY_CANVAS 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="FreeGlutWorkspace.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/@Development/Compilers/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui $(IncludeSwitch)C:\@Development\freeglut\include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)wxmsw31u_stc $(LibrarySwitch)opengl32 $(LibrarySwitch)glu32 $(LibrarySwitch)freeglut 
ArLibs                 :=  "libwxmsw31u_stc" "opengl32" "glu32" "libfreeglut" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:\@Development\freeglut\lib\x64 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/@Development/Compilers/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
CC       := C:/@Development/Compilers/TDM-GCC-64/bin/gcc.exe
CXXFLAGS := -Wno-deprecated-declarations -g -O0 -std=c++14 -std=c++11 -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\@Development\wxWidgets-3.1.0
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(ObjectSuffix) 



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
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/FreeGlutWorkspace/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLContextCncPath.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextCncPath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(DependSuffix): ../CncControlerGui/3D/GLContextCncPath.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLContextCncPath.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLContextCncPath.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextCncPath.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLContextCncPath.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLContextTestCube.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextTestCube.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(DependSuffix): ../CncControlerGui/3D/GLContextTestCube.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLContextTestCube.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLContextTestCube.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextTestCube.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLContextTestCube.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLContextBase.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(DependSuffix): ../CncControlerGui/3D/GLContextBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLContextBase.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLContextBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextBase.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLContextBase.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLContextPathBase.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLContextPathBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(DependSuffix): ../CncControlerGui/3D/GLContextPathBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLContextPathBase.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLContextPathBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLContextPathBase.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLContextPathBase.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(ObjectSuffix): ../CncControlerGui/3D/GLViewPort.cpp $(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/3D/GLViewPort.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(DependSuffix): ../CncControlerGui/3D/GLViewPort.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(DependSuffix) -MM ../CncControlerGui/3D/GLViewPort.cpp

$(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(PreprocessSuffix): ../CncControlerGui/3D/GLViewPort.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_3D_GLViewPort.cpp$(PreprocessSuffix) ../CncControlerGui/3D/GLViewPort.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


