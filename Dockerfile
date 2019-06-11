FROM centos:centos7.5.1804

RUN useradd -ms /bin/bash module_sdk
WORKDIR /home/module_sdk
COPY --chown=module_sdk:module_sdk . .
ARG CMAKE_VERSION="3.12.3"

RUN yum -y install \
            wget \
            tar \
            make \
            screen \
            nano \
            zlib-devel \
            centos-release-scl && \
    yum -y install \
            devtoolset-8-gcc \
            devtoolset-8-gcc-c++ && \
    export PATH="/opt/rh/devtoolset-8/root/usr/bin:$PATH" && \
    export CMAKE_VERSION="3.12.3" && \
    source scl_source enable devtoolset-8 && \
    mkdir -p /tmp/mtasa && \
    pushd /tmp/mtasa && \
    wget https://cmake.org/files/v3.12/cmake-${CMAKE_VERSION}.tar.gz && \
    tar xzf cmake-${CMAKE_VERSION}.tar.gz && \
    cmake-${CMAKE_VERSION}/bootstrap --parallel=6 --prefix=/usr/local && \
    gmake -j 6 && \
    make install && \
    wget https://linux.mtasa.com/dl/multitheftauto_linux_x64.tar.gz -O mta_server.tar.gz && \
    wget http://linux.mtasa.com/dl/baseconfig.tar.gz -O config.tar.gz && \
    tar -xvf mta_server.tar.gz && \
    tar -xvf config.tar.gz && \
    popd && \
    mkdir -p tests/mtasa_server && \
    mkdir -p tests/mtasa_server/mods/deathmatch && \
    #rsync -avh --ignore-existing /tmp/mtasa/multitheftauto_linux_x64/ tests/mtasa_server/ && \
    cp -nr /tmp/mtasa/multitheftauto_linux_x64/* tests/mtasa_server/ && \
    #rsync -avh --ignore-existing /tmp/mtasa/baseconfig/ tests/mtasa_server/mods/deathmatch/ && \
    cp -nr /tmp/mtasa/baseconfig/* tests/mtasa_server/mods/deathmatch/ && \
    ./build.sh TEST ON && \
    find . | xargs chown module_sdk:module_sdk && \
    yum clean all && \
    rm -rf /var/cache/yum && \
    rm -rf .git && \
    rm -rf /tmp/mtasa

# TODO: Remove nano, clear install

USER module_sdk

# Compile and install MTA-SA module
# RUN ./build.sh TEST

# RUN yum clean all && \
#     rm -rf /var/cache/yum && \
#     rm -rf .git && \
#     rm -rf /tmp/mtasa

CMD /bin/bash