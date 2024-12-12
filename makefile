all:
	@cmake -G Ninja -B ./build -S .
	@ninja -C ./build

clean:
	@rm -rf ./build
	@cmake -G Ninja -B ./build -S .
	@ninja -C ./build

run:
	@cmake -G Ninja -B ./build -S .
	@ninja -C ./build
	@./build/gmap-gui

crosscompilewin:
	@cmake -G Ninja -B ./buildwin -S . -DCMAKE_TOOLCHAIN_FILE=./TC/tc-mingw.cmake
	@ninja -C ./buildwin