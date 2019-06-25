FROM toliak/mtasa:latest

USER root
RUN apt-get -y update && \
    apt-get -y install procps \
                       screen

COPY run-tests.sh run-tests.sh
USER mtasa
CMD ./run-tests.sh
