.PHONY: clean All

All:
	@echo "----------Building project:[ CncConrollerGui - Debug ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk"
clean:
	@echo "----------Cleaning project:[ CncConrollerGui - Debug ]----------"
	@cd "CncControlerGui" && "$(MAKE)" -f  "CncConrollerGui.mk" clean
