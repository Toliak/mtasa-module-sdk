FROM debian:9.9-slim

RUN useradd -ms /bin/bash builder
WORKDIR /home/builder

# Prepare compilation environment
RUN apt-get -y update && \
    apt-get -y install gcc g++ cmake make && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    rm -rf /var/lib/apt

# Compile and save
ARG BUILD_ARGS
COPY --chown=builder:builder . .
RUN ./build.sh $BUILD_ARGS && \
    rm -rf bin

USER builder
VOLUME /home/builder/bin
CMD ./save-binary.sh bin
