all:
	@mkdir -p build
	@cmake -G Ninja -B ./build -S .
	@ninja -C ./build

clean:
	@rm -rf ./build
	@mkdir -p build
	@cmake -G Ninja -B ./build -S .
	@ninja -C ./build

run:
	@ninja -C ./build
	@./build/gmap-gui

crosscompilewin:
	@rm -rf ./buildwin
	@mkdir -p buildwin
	@cmake -G Ninja -B ./buildwin -S . -DCMAKE_TOOLCHAIN_FILE=./TC/tc-mingw.cmake
	@ninja -C ./buildwin