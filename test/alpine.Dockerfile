FROM alpine:3.13

RUN apk update && \
    apk add \
    bash git cmake gcc g++ make linux-headers

RUN mkdir -p /work/src
ADD [ ".", "/work/src" ]

RUN mkdir -p /work/build && \
    cd /work/build && \
    cmake /work/src && \
    cmake --build . && \
    ctest
