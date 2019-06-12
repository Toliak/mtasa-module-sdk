FROM debian:9.9-slim

RUN useradd -ms /bin/bash module_sdk
WORKDIR /home/module_sdk
COPY --chown=module_sdk:module_sdk . .

RUN apt-get update && \
    apt-get install -y --no-install-recommends wget \
                       make \
                       g++ \
                       gcc \
                       cmake \
                       procps \
                       screen && \
    mkdir -p /tmp/mtasa && \
    cd /tmp/mtasa && \
    wget http://linux.mtasa.com/dl/multitheftauto_linux_x64.tar.gz -O mta_server.tar.gz && \
    wget http://linux.mtasa.com/dl/baseconfig.tar.gz -O config.tar.gz && \
    tar -xvf mta_server.tar.gz && \
    tar -xvf config.tar.gz && \
    cd /home/module_sdk/tests && \
    mkdir -p mtasa_server && \
    cp -nr /tmp/mtasa/multitheftauto_linux_x64/* mtasa_server/ && \
    cp -nr /tmp/mtasa/baseconfig/* mtasa_server/mods/deathmatch/ && \
    cd /home/module_sdk/ && \
    ./build.sh TEST ON && \
    rm -rf /home/module_sdk/*build* && \
    find . | xargs chown module_sdk:module_sdk && \
    apt-get purge -y gcc g++ wget cmake && \
    apt-get clean -y && \
    apt-get autoremove -y && \
    rm -rf /home/module_sdk/.git && \
    rm -rf /tmp/mtasa && \
    rm -rf /var/lib/apt && \
    rm -rf /var/lib/dpkg && \
    rm -rf /usr/share/locale && \
    rm -rf /usr/share/man && \
    rm -rf /usr/share/doc

USER module_sdk
WORKDIR /home/module_sdk/tests
CMD ./run-tests.sh