.PHONY: clean All

All:
	@echo "----------Building project:[ CncConrollerGui - release2 ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" PreBuild && "$(MAKE)" -f  "CncConrollerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncConrollerGui - release2 ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" clean
