.PHONY: all

all:
	cmake -S ./src -B ./build
	cmake --build ./build --target all
	cd ./build && ./icp_project

doxygen:
	mkdir -p doc && doxygen

pack:
	zip -r xkrato67-xivanu00.zip *

clean:
	rm -rf doc/ build/ xkrato67-xivanu00.zip src/log.txt
