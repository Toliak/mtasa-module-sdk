FROM toliak/mtasa:latest

ARG GROUP_ID
ARG USER_ID
ENV GROUP_ID=1000
ENV USER_ID=1000

USER root
RUN id -g ${GROUP_ID} &>/dev/null || groupadd -g ${GROUP_ID} && \
    id -u ${USER_ID} &>/dev/null || useradd -u ${USER_ID}
RUN apt-get -y update && \
    apt-get -y install procps \
                       screen

COPY run-tests.sh run-tests.sh
USER ${USER_ID}
CMD ./run-tests.sh
