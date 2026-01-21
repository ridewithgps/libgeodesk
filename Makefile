# libgeodesk Makefile - CMake wrapper
#
# Usage:
#   make debug         - Build debug version
#   make release       - Build release version
#   make clean         - Remove build artifacts
#   make install       - Install library (release)
#
# With optional features:
#   make release GEOS=ON          - Build with GEOS support
#   make release OGR=ON           - Build with OGR/GDAL support
#   make release GEOS=ON OGR=ON   - Build with both
#
# Docker:
#   make docker        - Build Docker image with GEOS and OGR
#   make docker-shell  - Build and run interactive shell

BUILD_DIR := build
IMAGE_NAME := libgeodesk

# Parallel jobs (auto-detect or override with: make release JOBS=24)
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Default options (OFF for local builds - enable if you have deps installed)
GEOS ?= OFF
OGR ?= OFF

# Base CMake options
CMAKE_OPTS := -DGEODESK_MULTITHREADED=ON \
              -DGEODESK_WITH_GEOS=$(GEOS) \
              -DGEODESK_WITH_OGR=$(OGR)

# Default target
.PHONY: all
all: release

# Debug build
.PHONY: debug
debug:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug $(CMAKE_OPTS)
	cmake --build $(BUILD_DIR) --config Debug -j$(JOBS)

# Release build
.PHONY: release
release:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release $(CMAKE_OPTS)
	cmake --build $(BUILD_DIR) --config Release -j$(JOBS)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Install (uses release build)
.PHONY: install
install: release
	cmake --install $(BUILD_DIR)

# Docker: build image with GEOS and OGR
.PHONY: docker
docker:
	docker build -t $(IMAGE_NAME) .

# Docker: build and run interactive shell
.PHONY: docker-shell
docker-shell: docker
	docker run -it --rm -v $(PWD):/src $(IMAGE_NAME) bash
