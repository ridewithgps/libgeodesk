# libgeodesk Docker build with GEOS and GDAL support
#
# Build:
#   docker build -t libgeodesk .
#
# Run interactively:
#   docker run -it --rm -v $(pwd):/src libgeodesk bash

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies (runtime libs come as dependencies)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgeos++-dev \
    libgdal-dev \
    && rm -rf /var/lib/apt/lists/*

# Build arguments
ARG BUILD_TYPE=Release
ARG WITH_GEOS=ON
ARG WITH_OGR=ON

WORKDIR /src

COPY . .

# Configure and build
RUN cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DGEODESK_MULTITHREADED=ON \
    -DGEODESK_WITH_GEOS=${WITH_GEOS} \
    -DGEODESK_WITH_OGR=${WITH_OGR}

RUN cmake --build build --config ${BUILD_TYPE} -j$(nproc)

RUN cmake --install build
