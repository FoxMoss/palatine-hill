FROM debian:trixie-slim

RUN apt-get update && apt-get install -y \
    curl \
    libsqlite3-0 \
    libmbedtls21 \
    libmbedx509-7 \
    libmbedcrypto16 \
    libstdc++6 \
    libgcc-s1 \
    libc6 \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN curl -L https://github.com/FoxMoss/palatine-hill/releases/download/v0.0.12/bundle.tar.gz \
    -o bundle.tar.gz \
    && tar -xzf bundle.tar.gz \
    && rm bundle.tar.gz \ 
    && chmod +x sftp.sh

CMD ./sftp.sh $sftp_username $sftp_password; "./palatine"
