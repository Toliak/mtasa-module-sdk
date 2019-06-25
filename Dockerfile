FROM debian:9.9-slim

RUN useradd -ms /bin/bash builder
WORKDIR /home/builder
COPY --chown=builder:builder . .

ARG BUILD_ARGS
RUN apt-get -y update && \
    apt-get -y install gcc g++ cmake make && \
    ./build.sh $BUILD_ARGS && \
    apt-get -y purge gcc g++ cmake make && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    rm -rf /var/lib/apt

USER builder
VOLUME /home/builder/bin
CMD ./save-binary.sh
