##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=CNCGuiController
ConfigurationName      :=Release
WorkspaceConfiguration := $(ConfigurationName)
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CNCGuiController
IntermediateDirectory  :=../build-$(ConfigurationName)/CNCGuiController
OutDir                 :=../build-$(ConfigurationName)/CNCGuiController
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=15/06/2020
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/g++.exe -shared -fPIC
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
OutputFile             :=..\build-$(ConfigurationName)\bin\$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := 
RcCompilerName         :=C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/windres.exe
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/g++.exe
CC       := C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/gcc.exe
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/mingw-w64/x86_64-6.4.0-posix-seh-rt_v5-rev0/mingw64/bin/as.exe


##
## User defined environment variables
##
Objects0=../build-$(ConfigurationName)/CNCGuiController/main.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): ../build-$(ConfigurationName)/CNCGuiController/.d $(Objects) 
	@if not exist "..\build-$(ConfigurationName)\CNCGuiController" mkdir "..\build-$(ConfigurationName)\CNCGuiController"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "..\build-$(ConfigurationName)\CNCGuiController" mkdir "..\build-$(ConfigurationName)\CNCGuiController"
	@if not exist ""..\build-$(ConfigurationName)\bin"" mkdir ""..\build-$(ConfigurationName)\bin""

../build-$(ConfigurationName)/CNCGuiController/.d:
	@if not exist "..\build-$(ConfigurationName)\CNCGuiController" mkdir "..\build-$(ConfigurationName)\CNCGuiController"

PreBuild:


##
## Objects
##
../build-$(ConfigurationName)/CNCGuiController/main.cpp$(ObjectSuffix): main.cpp ../build-$(ConfigurationName)/CNCGuiController/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CNCGuiController/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/CNCGuiController/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/CNCGuiController/main.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/CNCGuiController/main.cpp$(DependSuffix) -MM main.cpp

../build-$(ConfigurationName)/CNCGuiController/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/CNCGuiController/main.cpp$(PreprocessSuffix) main.cpp


-include ../build-$(ConfigurationName)/CNCGuiController//*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


