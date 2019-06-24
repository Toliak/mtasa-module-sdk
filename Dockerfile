FROM debian:9.9-slim

ARG GROUP_ID
ARG USER_ID
ENV GROUP_ID=1000
ENV USER_ID=1000

RUN groupadd -g ${GROUP_ID} builder && \
        useradd -u ${USER_ID} -g builder builder
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
