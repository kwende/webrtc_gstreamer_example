FROM ubuntu:18.04
USER root

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install vim git build-essential unzip \
        python3 python3-setuptools sudo help2man -y

RUN git clone https://gitlab.freedesktop.org/gstreamer/cerbero.git && \
    cd cerbero && \
    git checkout 1.16 && \
    echo "Y" | ./cerbero-uninstalled -t -c config/cross-android-universal.cbc bootstrap && \
    ./cerbero-uninstalled -t -c config/cross-android-universal.cbc package gstreamer-1.0

COPY ./recipes /cerbero/recipes/

RUN cd /cerbero && \
    ./cerbero-uninstalled -t -c config/cross-android-universal.cbc build glib
    
CMD tail -f /dev/null