##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=CNCUnitTests
ConfigurationName      :=Release
WorkspaceConfiguration :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/CNCUnitTests
IntermediateDirectory  :=$(ConfigurationName)
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=01/01/2023
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).exe
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="CNCUnitTests.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/msys64/mingw64/bin/windres.exe
LinkOptions            :=  -O2 $(shell wx-config --libs) 
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)C:/@Development/unittest-cpp $(IncludeSwitch)C:/@Development/boost_1_79_0 $(IncludeSwitch)C:/@Development/wxWidgets-3.2.1/include $(IncludeSwitch)C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui $(IncludeSwitch)C:/msys64/mingw64/include $(IncludeSwitch)C:/@Development/wxsvg-1.5.24/include $(IncludeSwitch)C:/@Development/opencv/build/include $(IncludeSwitch)C:/@Development/openvg 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)UnitTest++ $(LibrarySwitch)wxmsw32u_stc $(LibrarySwitch)wxmsw32u_webview $(LibrarySwitch)wxmsw32u_propgrid $(LibrarySwitch)wxmsw32u_adv $(LibrarySwitch)wxmsw32u_gl $(LibrarySwitch)wxmsw32u_aui $(LibrarySwitch)wxsvg $(LibrarySwitch)cairo $(LibrarySwitch)avutil $(LibrarySwitch)avformat $(LibrarySwitch)avcodec $(LibrarySwitch)xvidcore $(LibrarySwitch)expat $(LibrarySwitch)exif $(LibrarySwitch)swscale $(LibrarySwitch)backtrace $(LibrarySwitch)dl $(LibrarySwitch)dbgeng $(LibrarySwitch)opengl32 $(LibrarySwitch)strmiids $(LibrarySwitch)glu32 $(LibrarySwitch)glew32 $(LibrarySwitch)ole32 $(LibrarySwitch)oleaut32 $(LibrarySwitch)freeglut $(LibrarySwitch)Xinput9_1_0 $(LibrarySwitch)opencv_core451 $(LibrarySwitch)opencv_video451 $(LibrarySwitch)opencv_videoio451 $(LibrarySwitch)opencv_highgui451 
ArLibs                 :=  "libUnitTest++.a" "libwxmsw32u_stc" "libwxmsw32u_webview" "libwxmsw32u_propgrid" "libwxmsw32u_adv" "libwxmsw32u_gl" "libwxmsw32u_aui" "libwxsvg" "libcairo" "libavutil" "libavformat" "libavcodec" "libxvidcore" "libexpat" "libexif" "libswscale" "backtrace" "libdl" "dbgeng" "opengl32" "strmiids" "glu32" "glew32" "ole32" "oleaut32" "freeglut" "Xinput9_1_0" "libopencv_core451" "libopencv_video451" "libopencv_videoio451" "libopencv_highgui451" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)C:\@Development\unittest-cpp\builds. $(LibraryPathSwitch)C:/@Development/wxWidgets-3.2.1/lib/gcc_dll $(LibraryPathSwitch)C:/msys64/mingw64/lib $(LibraryPathSwitch)C:/@Development/opencv/mingwbuild/lib 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := C:/msys64/mingw64/bin/ar.exe rcu
CXX      := C:/msys64/mingw64/bin/g++.exe
CC       := C:/msys64/mingw64/bin/gcc.exe
CXXFLAGS :=  -g1 -fpic -O3 -std=c++2a -Wall $(shell wx-config --cflags) $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := C:/msys64/mingw64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Test_CncPosition.cpp$(ObjectSuffix) $(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(ObjectSuffix) 



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

PostBuild:
	@echo Executing Post Build commands ...
	C:\@Development\@Projekte\c++\CNCGuiController\CNCUnitTests\Release\CNCUnitTests.exe
	@echo Done

MakeIntermediateDirs:
	@$(MakeDirCommand) "$(ConfigurationName)"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "$(ConfigurationName)"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(ObjectSuffix): ../CncControlerGui/CncVector.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(DependSuffix) -MM ../CncControlerGui/CncVector.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncVector.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(PreprocessSuffix): ../CncControlerGui/CncVector.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncVector.cpp$(PreprocessSuffix) ../CncControlerGui/CncVector.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(ObjectSuffix): ../CncControlerGui/CncPosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(DependSuffix) -MM ../CncControlerGui/CncPosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncPosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(PreprocessSuffix): ../CncControlerGui/CncPosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncPosition.cpp$(PreprocessSuffix) ../CncControlerGui/CncPosition.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(ObjectSuffix): ../CncControlerGui/CncSimpleCalculator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(DependSuffix) -MM ../CncControlerGui/CncSimpleCalculator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncSimpleCalculator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(PreprocessSuffix): ../CncControlerGui/CncSimpleCalculator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncSimpleCalculator.cpp$(PreprocessSuffix) ../CncControlerGui/CncSimpleCalculator.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CNCUnitTests/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/Test_CncPosition.cpp$(ObjectSuffix): Test_CncPosition.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Test_CncPosition.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Test_CncPosition.cpp$(DependSuffix) -MM Test_CncPosition.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CNCUnitTests/Test_CncPosition.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Test_CncPosition.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Test_CncPosition.cpp$(PreprocessSuffix): Test_CncPosition.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Test_CncPosition.cpp$(PreprocessSuffix) Test_CncPosition.cpp

$(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(ObjectSuffix): Test_CncSimpleCalculator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(DependSuffix) -MM Test_CncSimpleCalculator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CNCUnitTests/Test_CncSimpleCalculator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(PreprocessSuffix): Test_CncSimpleCalculator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Test_CncSimpleCalculator.cpp$(PreprocessSuffix) Test_CncSimpleCalculator.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ConfigurationName)/


