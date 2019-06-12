FROM debian:9

RUN useradd -ms /bin/bash module_sdk
WORKDIR /home/module_sdk
COPY --chown=module_sdk:module_sdk . .

RUN apt-get update && \
    apt-get install -y wget \
                       cmake \
                       g++ \
                       gcc \
                       screen && \
    mkdir -p /tmp/mtasa && \
    cd /tmp/mtasa && \
    wget https://linux.mtasa.com/dl/multitheftauto_linux_x64.tar.gz -O mta_server.tar.gz && \
    wget http://linux.mtasa.com/dl/baseconfig.tar.gz -O config.tar.gz && \
    tar -xvf mta_server.tar.gz && \
    tar -xvf config.tar.gz && \
    cd /home/module_sdk/tests && \
    mkdir -p mtasa_server && \
    cp -nr /tmp/mtasa/multitheftauto_linux_x64/* mtasa_server/ && \
    cp -nr /tmp/mtasa/baseconfig/* mtasa_server/mods/deathmatch/ && \
    cd /home/module_sdk/ && \
    ./build.sh TEST ON && \
    find . | xargs chown module_sdk:module_sdk && \
    rm -rf /home/module_sdk/.git && \
    rm -rf /tmp/mtasa

USER module_sdk
WORKDIR /home/module_sdk/tests
CMD ./run-tests.sh