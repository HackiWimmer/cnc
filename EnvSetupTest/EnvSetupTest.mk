##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=EnvSetupTest
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=27/04/2022
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
ObjectsFileList        :="EnvSetupTest.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  $(shell wx-config --libs) -mwindows
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/@Development/boost_1_77_0 $(IncludeSwitch)C:/@Development/wxsvg-1.5.22/include $(IncludeSwitch)C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)wxsvg $(LibrarySwitch)cairo $(LibrarySwitch)avutil $(LibrarySwitch)avformat $(LibrarySwitch)avcodec $(LibrarySwitch)xvidcore $(LibrarySwitch)expat $(LibrarySwitch)exif $(LibrarySwitch)swscale 
ArLibs                 :=  "libwxsvg" "libcairo" "libavutil" "libavformat" "libavcodec" "libxvidcore" "libexpat" "libexif" "libswscale" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:/msys64/mingw64/lib $(LibraryPathSwitch)C:/@Development/opencv/mingwbuild/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
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
Objects0=$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_test.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) $(IntermediateDirectory)/MainDialog.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) $(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) 



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
$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix): ../CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(DependSuffix) -MM ../CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(PreprocessSuffix): ../CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel_bitmaps.cpp$(PreprocessSuffix) ../CncControlerGui/wxCrafterSecurePanel_bitmaps.cpp

$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(ObjectSuffix): ../CncControlerGui/wxCrafterSecurePanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(DependSuffix) -MM ../CncControlerGui/wxCrafterSecurePanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/wxCrafterSecurePanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(PreprocessSuffix): ../CncControlerGui/wxCrafterSecurePanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_wxCrafterSecurePanel.cpp$(PreprocessSuffix) ../CncControlerGui/wxCrafterSecurePanel.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(ObjectSuffix): ../CncControlerGui/CncSecureCtrlPanel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(DependSuffix) -MM ../CncControlerGui/CncSecureCtrlPanel.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSecureCtrlPanel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(PreprocessSuffix): ../CncControlerGui/CncSecureCtrlPanel.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncSecureCtrlPanel.cpp$(PreprocessSuffix) ../CncControlerGui/CncSecureCtrlPanel.cpp

$(IntermediateDirectory)/up_CncControlerGui_test.cpp$(ObjectSuffix): ../CncControlerGui/test.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_test.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_test.cpp$(DependSuffix) -MM ../CncControlerGui/test.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/test.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_test.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_test.cpp$(PreprocessSuffix): ../CncControlerGui/test.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_test.cpp$(PreprocessSuffix) ../CncControlerGui/test.cpp

$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix): wxcrafter_bitmaps.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(DependSuffix) -MM wxcrafter_bitmaps.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/wxcrafter_bitmaps.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(PreprocessSuffix): wxcrafter_bitmaps.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxcrafter_bitmaps.cpp$(PreprocessSuffix) wxcrafter_bitmaps.cpp

$(IntermediateDirectory)/MainDialog.cpp$(ObjectSuffix): MainDialog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MainDialog.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MainDialog.cpp$(DependSuffix) -MM MainDialog.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/MainDialog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MainDialog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MainDialog.cpp$(PreprocessSuffix): MainDialog.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MainDialog.cpp$(PreprocessSuffix) MainDialog.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix): wxcrafter.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/wxcrafter.cpp$(DependSuffix) -MM wxcrafter.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/wxcrafter.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/wxcrafter.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/wxcrafter.cpp$(PreprocessSuffix): wxcrafter.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/wxcrafter.cpp$(PreprocessSuffix) wxcrafter.cpp

$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix): win_resources.rc
	$(RcCompilerName) -i "C:/@Development/@Projekte/c++/CNCGuiController/EnvSetupTest/win_resources.rc" $(RcCmpOptions)   $(ObjectSwitch)$(IntermediateDirectory)/win_resources.rc$(ObjectSuffix) $(RcIncludePath)

-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


