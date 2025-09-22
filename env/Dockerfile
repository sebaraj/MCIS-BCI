FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    python3-pip \
    git \
    && pip3 install --no-cache-dir --break-system-packages conan \
    && rm -rf /var/lib/apt/lists/*

RUN conan profile detect

WORKDIR /workspace

COPY . /workspace

