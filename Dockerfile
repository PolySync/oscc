FROM ubuntu:16.04

WORKDIR /app

# common packages
RUN apt-get update && \
    apt-get install -y \
    ca-certificates curl file tar clang libclang-dev \
    build-essential cmake libssl-dev zlib1g-dev \
    ruby-dev libboost-dev git wget && \
    rm -rf /var/lib/apt/lists/*

# install cucumber package
RUN gem install cucumber -v 2.0.0 --no-rdoc --no-ri

# install rust toolchain
env RUSTUP_HOME=/usr/local/rustup \
    CARGO_HOME=/usr/local/cargo

RUN curl https://sh.rustup.rs -sSf | \
    sh -s -- --default-toolchain 1.20.0 -y

env PATH=/usr/local/cargo/bin:${PATH}

# install arduino toolchain
RUN wget -nv http://arduino.cc/download.php?f=/arduino-1.8.5-linux64.tar.xz -O arduino-1.8.5.tar.xz

RUN tar -xf arduino-1.8.5.tar.xz && \
    cd arduino-1.8.5 && \
    mkdir -p /usr/share/arduino && \
    cp -R * /usr/share/arduino


# Fetch and build cargo deps
RUN mkdir src && echo "fn main() { }" >> build.rs && touch src/tests.rs

COPY ./firmware/brake/kia_soul_ev_niro/tests/property/Cargo.toml .
RUN cargo build

COPY ./firmware/brake/kia_soul_petrol/tests/property/Cargo.toml .
RUN cargo build

COPY ./firmware/common/libs/pid/tests/property/Cargo.toml .
RUN cargo build

COPY ./firmware/steering/tests/property/Cargo.toml .
RUN cargo build

COPY ./firmware/throttle/tests/property/Cargo.toml .
RUN cargo build
