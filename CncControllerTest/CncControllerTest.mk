##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=CncControllerTest
ConfigurationName      :=Release
WorkspaceConfiguration := $(ConfigurationName)
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest
IntermediateDirectory  :=../build-$(ConfigurationName)/CncControllerTest
OutDir                 :=../build-$(ConfigurationName)/CncControllerTest
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=29/09/2020
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe
SharedObjectLinkerName :=C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe -shared -fPIC
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
RcCompilerName         :=C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs)
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
AR       := C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe
CC       := C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe
CXXFLAGS :=  -O2 -Wall $(shell wx-config --cxxflags) $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/as.exe


##
## User defined environment variables
##
Objects0=../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): ../build-$(ConfigurationName)/CncControllerTest/.d $(Objects) 
	@if not exist "..\build-$(ConfigurationName)\CncControllerTest" mkdir "..\build-$(ConfigurationName)\CncControllerTest"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "..\build-$(ConfigurationName)\CncControllerTest" mkdir "..\build-$(ConfigurationName)\CncControllerTest"
	@if not exist ""..\build-$(ConfigurationName)\bin"" mkdir ""..\build-$(ConfigurationName)\bin""

../build-$(ConfigurationName)/CncControllerTest/.d:
	@if not exist "..\build-$(ConfigurationName)\CncControllerTest" mkdir "..\build-$(ConfigurationName)\CncControllerTest"

PreBuild:


##
## Objects
##
../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(ObjectSuffix): main_app.cpp ../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest/main_app.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main_app.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(DependSuffix): main_app.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(DependSuffix) -MM main_app.cpp

../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(PreprocessSuffix): main_app.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/CncControllerTest/main_app.cpp$(PreprocessSuffix) main_app.cpp

../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(ObjectSuffix): impl.cpp ../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControllerTest/impl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/impl.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(DependSuffix): impl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(DependSuffix) -MM impl.cpp

../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(PreprocessSuffix): impl.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/CncControllerTest/impl.cpp$(PreprocessSuffix) impl.cpp


-include ../build-$(ConfigurationName)/CncControllerTest//*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


