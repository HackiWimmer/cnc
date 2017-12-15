##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=Experiments
ConfigurationName      :=Release
WorkspacePath          :=C:/@Development/@Projekte/c++/CNCGuiController
ProjectPath            :=C:/@Development/@Projekte/c++/CNCGuiController/Experiments
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Hacki
Date                   :=15/12/2017
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
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Experiments.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := $(shell wx-config --rcflags)
RcCompilerName         :=C:/@Development/Compilers/TDM-GCC-64/bin/windres.exe
LinkOptions            :=  -s $(shell wx-config --debug=no --libs --unicode=yes)
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)C:\@Development\@Projekte\c++\CNCGuiController\CncControlerGui 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := C:/@Development/Compilers/TDM-GCC-64/bin/ar.exe rcu
CXX      := C:/@Development/Compilers/TDM-GCC-64/bin/g++.exe
CC       := C:/@Development/Compilers/TDM-GCC-64/bin/gcc.exe
CXXFLAGS := -std=c++14 -std=c++11 -O2 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(shell wx-config --cxxflags --unicode=yes --debug=no) $(Preprocessors)
ASFLAGS  := 
AS       := C:/@Development/Compilers/TDM-GCC-64/bin/as.exe


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
WXWIN:=C:\@Development\wxWidgets-3.1.0
WXCFG:=gcc_dll\mswu
Objects0=$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(ObjectSuffix) $(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(ObjectSuffix) $(IntermediateDirectory)/mainx.cpp$(ObjectSuffix) 



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
$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(ObjectSuffix): ../CncControlerGui/CncCommon.cpp $(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncCommon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(DependSuffix): ../CncControlerGui/CncCommon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(DependSuffix) -MM ../CncControlerGui/CncCommon.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(PreprocessSuffix): ../CncControlerGui/CncCommon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncCommon.cpp$(PreprocessSuffix) ../CncControlerGui/CncCommon.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(ObjectSuffix): ../CncControlerGui/CncTimeFunctions.cpp $(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/CncControlerGui/CncTimeFunctions.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(DependSuffix): ../CncControlerGui/CncTimeFunctions.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(DependSuffix) -MM ../CncControlerGui/CncTimeFunctions.cpp

$(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(PreprocessSuffix): ../CncControlerGui/CncTimeFunctions.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/up_CncControlerGui_CncTimeFunctions.cpp$(PreprocessSuffix) ../CncControlerGui/CncTimeFunctions.cpp

$(IntermediateDirectory)/mainx.cpp$(ObjectSuffix): mainx.cpp $(IntermediateDirectory)/mainx.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/@Development/@Projekte/c++/CNCGuiController/Experiments/mainx.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/mainx.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/mainx.cpp$(DependSuffix): mainx.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/mainx.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/mainx.cpp$(DependSuffix) -MM mainx.cpp

$(IntermediateDirectory)/mainx.cpp$(PreprocessSuffix): mainx.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/mainx.cpp$(PreprocessSuffix) mainx.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


