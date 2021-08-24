FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && \
    apt-get install -y \
    bash build-essential git cmake

RUN mkdir -p /work/src
ADD [ ".", "/work/src" ]

RUN mkdir -p /work/build && \
    cd /work/build && \
    cmake /work/src && \
    cmake --build .
