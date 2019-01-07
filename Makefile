.PHONY: clean All

All:
	@echo "----------Building project:[ NavigatorPanel - Release ]----------"
	@cd "NavigatorPanel" && "$(MAKE)" -f  "NavigatorPanel.mk"
clean:
	@echo "----------Cleaning project:[ NavigatorPanel - Release ]----------"
	@cd "NavigatorPanel" && "$(MAKE)" -f  "NavigatorPanel.mk" clean
