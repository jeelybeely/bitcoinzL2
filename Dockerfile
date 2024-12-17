FROM debian:stretch-slim as build

# Fix sources.list to use archive.debian.org and remove stretch-updates
RUN sed -i 's|http://deb.debian.org/debian|http://archive.debian.org/debian|g' /etc/apt/sources.list && \
    sed -i 's|http://security.debian.org/debian-security|http://archive.debian.org/debian-security|g' /etc/apt/sources.list && \
    sed -i '/stretch-updates/d' /etc/apt/sources.list && \
    echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99no-check-valid-until && \
    apt-get update \
  && apt-get install -y \
    curl \
    build-essential \
    pkg-config \
    libc6-dev \
    m4 \
    g++-multilib \
    autoconf \
    libtool \
    ncurses-dev \
    unzip \
    git \
    python \
    zlib1g-dev \
    wget \
    bsdmainutils \
    automake \
    p7zip-full \
    pwgen \
    dos2unix \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /usr/local/src/
COPY . /usr/local/src/

# Fix line endings in build.sh
RUN dos2unix ./zcutil/build.sh

RUN ./zcutil/build.sh -j$(nproc)
RUN ./zcutil/fetch-params.sh

FROM debian:stretch-slim

# Fix sources.list to use archive.debian.org and remove stretch-updates
RUN sed -i 's|http://deb.debian.org/debian|http://archive.debian.org/debian|g' /etc/apt/sources.list && \
    sed -i 's|http://security.debian.org/debian-security|http://archive.debian.org/debian-security|g' /etc/apt/sources.list && \
    sed -i '/stretch-updates/d' /etc/apt/sources.list && \
    echo 'Acquire::Check-Valid-Until "false";' > /etc/apt/apt.conf.d/99no-check-valid-until && \
    groupadd -r bitcoinz && useradd -r -d /bitcoinz -g bitcoinz bitcoinz

# Install our run dependencies
RUN apt-get update \
  && apt-get install -y \
    curl \
    libc6-dev \
    g++-multilib \
  && rm -rf /var/lib/apt/lists/*

# Setup application directory
RUN mkdir -p /bitcoinz/data

# Copy binaries from build container
COPY --from=build /usr/local/src/src/bitcoinzd /usr/local/bin
COPY --from=build /usr/local/src/src/bitcoinz-cli /usr/local/bin
COPY --from=build /usr/local/src/src/zcash-gtest /usr/local/bin
COPY --from=build /usr/local/src/src/bitcoinz-tx /usr/local/bin
COPY contrib/docker/cli /usr/local/bin

RUN chmod +x /usr/local/bin/cli

# Copy zcash params
COPY --from=build /root/.zcash-params /bitcoinz/.zcash-params

RUN chown -R bitcoinz: /bitcoinz
USER bitcoinz
WORKDIR /bitcoinz
CMD ["bitcoinzd", "-datadir=/bitcoinz/data"]
