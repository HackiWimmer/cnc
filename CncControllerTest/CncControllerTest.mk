##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=CncControllerTest
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=15/07/2022
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
OutputDirectory        :=$(IntermediateDirectory)
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="CncControllerTest.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)sapi 
ArLibs                 :=  "sapi" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/msys64/mingw64/bin/ar.exe rcu
CXX      := C:/msys64/mingw64/bin/g++.exe
CC       := C:/msys64/mingw64/bin/gcc.exe
CXXFLAGS :=  -O2 -Wall $(shell wx-config --cxxflags) $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/msys64/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/main_app.cpp$(ObjectSuffix) $(IntermediateDirectory)/impl.cpp$(ObjectSuffix) $(IntermediateDirectory)/FtMonitor.cpp$(ObjectSuffix) 



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
	@$(MakeDirCommand) "$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main_app.cpp$(ObjectSuffix): main_app.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main_app.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main_app.cpp$(DependSuffix) -MM main_app.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest/main_app.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main_app.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main_app.cpp$(PreprocessSuffix): main_app.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main_app.cpp$(PreprocessSuffix) main_app.cpp

$(IntermediateDirectory)/impl.cpp$(ObjectSuffix): impl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/impl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/impl.cpp$(DependSuffix) -MM impl.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest/impl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/impl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/impl.cpp$(PreprocessSuffix): impl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/impl.cpp$(PreprocessSuffix) impl.cpp

$(IntermediateDirectory)/FtMonitor.cpp$(ObjectSuffix): FtMonitor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FtMonitor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FtMonitor.cpp$(DependSuffix) -MM FtMonitor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest/FtMonitor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FtMonitor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FtMonitor.cpp$(PreprocessSuffix): FtMonitor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FtMonitor.cpp$(PreprocessSuffix) FtMonitor.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


