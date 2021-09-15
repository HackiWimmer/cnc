##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=EnvSetupTest
ConfigurationName      :=Release
WorkspaceConfiguration := $(ConfigurationName)
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest
IntermediateDirectory  :=../build-$(ConfigurationName)/EnvSetupTest
OutDir                 :=../build-$(ConfigurationName)/EnvSetupTest
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=15/09/2021
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
OutputFile             :=..\build-$(ConfigurationName)\bin\$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs) -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/@Development/boost_1_77_0 $(IncludeSwitch)C:/@Development/wxsvg-1.5.22/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)wxsvg $(LibrarySwitch)cairo $(LibrarySwitch)avutil $(LibrarySwitch)avformat $(LibrarySwitch)avcodec $(LibrarySwitch)xvidcore $(LibrarySwitch)expat $(LibrarySwitch)exif $(LibrarySwitch)swscale 
ArLibs                 :=  "libwxsvg" "libcairo" "libavutil" "libavformat" "libavcodec" "libxvidcore" "libexpat" "libexif" "libswscale" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:/msys64/mingw64/lib $(LibraryPathSwitch)C:/@Development/opencv/mingwbuild/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/msys64/mingw64/bin/ar.exe rcu
CXX      := C:/msys64/mingw64/bin/g++.exe
CC       := C:/msys64/mingw64/bin/gcc.exe
CXXFLAGS :=  -O2 -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := C:/msys64/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=../build-$(ConfigurationName)/EnvSetupTest/win_resources.rc$(ObjectSuffix) ../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): ../build-$(ConfigurationName)/EnvSetupTest/.d $(Objects) 
	@if not exist "..\build-$(ConfigurationName)\EnvSetupTest" mkdir "..\build-$(ConfigurationName)\EnvSetupTest"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@if not exist "..\build-$(ConfigurationName)\EnvSetupTest" mkdir "..\build-$(ConfigurationName)\EnvSetupTest"
	@if not exist ""..\build-$(ConfigurationName)\bin"" mkdir ""..\build-$(ConfigurationName)\bin""

../build-$(ConfigurationName)/EnvSetupTest/.d:
	@if not exist "..\build-$(ConfigurationName)\EnvSetupTest" mkdir "..\build-$(ConfigurationName)\EnvSetupTest"

PreBuild:


##
## Objects
##
../build-$(ConfigurationName)/EnvSetupTest/win_resources.rc$(ObjectSuffix): win_resources.rc
	$(RcCompilerName) -i "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/win_resources.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(RcIncludePath)
../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(ObjectSuffix): MainDialog.cpp ../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/MainDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainDialog.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(DependSuffix): MainDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(DependSuffix) -MM MainDialog.cpp

../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(PreprocessSuffix): MainDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/EnvSetupTest/MainDialog.cpp$(PreprocessSuffix) MainDialog.cpp

../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(ObjectSuffix): wxcrafter_bitmaps.cpp ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/wxcrafter_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(DependSuffix): wxcrafter_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(DependSuffix) -MM wxcrafter_bitmaps.cpp

../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(PreprocessSuffix): wxcrafter_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter_bitmaps.cpp$(PreprocessSuffix) wxcrafter_bitmaps.cpp

../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(ObjectSuffix): wxcrafter.cpp ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/wxcrafter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(DependSuffix): wxcrafter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(DependSuffix) -MM wxcrafter.cpp

../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(PreprocessSuffix): wxcrafter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/EnvSetupTest/wxcrafter.cpp$(PreprocessSuffix) wxcrafter.cpp

../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(ObjectSuffix): main.cpp ../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(DependSuffix) -MM main.cpp

../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/EnvSetupTest/main.cpp$(PreprocessSuffix) main.cpp


-include ../build-$(ConfigurationName)/EnvSetupTest//*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


