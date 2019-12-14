.PHONY: clean All

All:
	@echo "----------Building project:[ CncConrollerGui - Gprof ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" PreBuild && "$(MAKE)" -f  "CncConrollerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncConrollerGui - Gprof ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" clean
