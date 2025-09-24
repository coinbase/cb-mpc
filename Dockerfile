FROM public.ecr.aws/docker/library/golang:1.24.6-bookworm

LABEL description="Build environment for MPC project with LLVM and tweaked OpenSSL"
LABEL version="1.0"

ENV CGO_ENABLED=0 \
    CC=/usr/bin/clang-20 \
    CXX=/usr/bin/clang++-20

RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl-dev \
    cmake \
    rsync \
    wget \
    gnupg \
    ca-certificates \
    lsb-release \
    && rm -rf /var/lib/apt/lists/* \
    # Import LLVM's GPG key
    && wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm-archive-keyring.gpg \
    # Add LLVM repository
    && echo "deb [signed-by=/usr/share/keyrings/llvm-archive-keyring.gpg] http://apt.llvm.org/$(lsb_release -cs)/ llvm-toolchain-$(lsb_release -cs)-20 main" > /etc/apt/sources.list.d/llvm20.list \
    # Install LLVM tools
    && apt-get update && apt-get install -y --no-install-recommends \
    clang-20 \
    clang-format-20 \
    lld-20 \
    libfuzzer-20-dev \
    libclang-rt-20-dev \
    && rm -rf /var/lib/apt/lists/* \
    # Set up clang-format
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-20 100

# Build and install tweaked OpenSSL using the build script
WORKDIR /build
COPY scripts/openssl/build-static-openssl-linux.sh .
RUN sh build-static-openssl-linux.sh \
    && mkdir -p /usr/local/lib64 /usr/local/lib /usr/local/include \
    && ln -sf /usr/local/opt/openssl@3.2.0/lib64/libcrypto.a /usr/local/lib64/libcrypto.a \
    && ln -sf /usr/local/opt/openssl@3.2.0/lib64/libcrypto.a /usr/local/lib/libcrypto.a \
    && ln -sf /usr/local/opt/openssl@3.2.0/include/openssl /usr/local/include/openssl \
    && rm -rf /build

WORKDIR /code
