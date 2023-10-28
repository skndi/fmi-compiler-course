FROM debian:latest

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get clean

RUN apt install spim build-essential -y

RUN apt install spim build-essential -y

RUN apt install csh flex -y

RUN mkdir /usr/class

RUN mkdir /usr/code

RUN useradd -ms /bin/bash nikolay

ENV PATH="cs143/bin:${PATH}"

WORKDIR /usr/code

CMD /bin/bash
