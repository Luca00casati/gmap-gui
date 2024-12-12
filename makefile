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
