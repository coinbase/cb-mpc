FROM golang:1.23

ENV CGO_ENABLED=0

RUN apt-get update && \
    apt-get install libssl-dev -y
RUN apt-get install cmake -y
RUN apt-get install rsync -y
RUN apt-get install clang-format -y

RUN mkdir -p /build
COPY scripts/openssl/build-static-openssl-linux.sh /build
WORKDIR /build
RUN sh build-static-openssl-linux.sh
RUN mkdir -p /usr/local/lib64
RUN mkdir -p /usr/local/lib
RUN mkdir -p /usr/local/include

RUN ln -sf /usr/local/opt/openssl@3.2.0/lib64/libcrypto.a /usr/local/lib64/libcrypto.a
RUN ln -sf /usr/local/opt/openssl@3.2.0/lib64/libcrypto.a /usr/local/lib/libcrypto.a
RUN ln -sf /usr/local/opt/openssl@3.2.0/include/openssl /usr/local/include/openssl

RUN mkdir /code
WORKDIR /code
RUN rm -rf /build
