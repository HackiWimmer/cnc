.PHONY: clean All

All:
	@echo "----------Building project:[ CncConrollerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" PreBuild && "$(MAKE)" -f  "CncConrollerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncConrollerGui - Release ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" clean
