FROM toliak/mtasa:latest

USER root
RUN apt-get -y update && \
    apt-get -y install procps && \
    apt-get -y autoremove && \
    apt-get -y clean && \
    rm -rf /var/lib/apt

COPY run-tests.sh run-tests.sh
USER mtasa
CMD ./run-tests.sh
