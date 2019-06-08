FROM centos:centos7.5.1804

RUN yum -y install wget tar make rsync zlib-devel centos-release-scl && \
    yum -y install devtoolset-8-gcc devtoolset-8-gcc-c++

ENV PATH="/opt/rh/devtoolset-8/root/usr/bin:$PATH"
RUN source scl_source enable devtoolset-8

WORKDIR /tmp/mtasa

# CMake
ARG CMAKE_VERSION="3.12.3"
RUN wget https://cmake.org/files/v3.12/cmake-${CMAKE_VERSION}.tar.gz && \
    tar xzf cmake-${CMAKE_VERSION}.tar.gz && \
    cmake-${CMAKE_VERSION}/bootstrap --parallel=6 --prefix=/usr/local && \
    gmake -j 6 && \
    make install

RUN useradd -ms /bin/bash module_sdk
# USER module_sdk

RUN wget https://linux.mtasa.com/dl/multitheftauto_linux_x64.tar.gz -O /tmp/mtasa/mta_server.tar.gz && \
    wget http://linux.mtasa.com/dl/baseconfig.tar.gz -O /tmp/mtasa/config.tar.gz

# COPY --chown=module_sdk:module_sdk . .

WORKDIR /home/module_sdk

COPY . .

RUN tar -xvf /tmp/mtasa/mta_server.tar.gz && \
    rsync -avh --ignore-existing multitheftauto_linux_x64/ tests/mtasa_server/ && \
    tar -xvf /tmp/mtasa/config.tar.gz && \
    rsync -avh --ignore-existing baseconfig/ tests/mtasa_server/mods/deathmatch/

RUN yum clean all && \
    rm -rf /var/cache/yum && \
    rm -rf .git && \
    rm -rf /tmp/mtasa

CMD /bin/bash