FROM debian:latest

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get clean

RUN apt install spim build-essential -y

RUN apt install spim build-essential -y

RUN apt install csh flex -y

RUN mkdir /usr/class

RUN mkdir /usr/code

ENV PATH="/usr/class/cs143/bin:${PATH}"

WORKDIR /usr/code

CMD /bin/bash
