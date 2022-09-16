FROM ubuntu:20.04

RUN apt update && DEBIAN_FRONTEND=noninteractive apt install build-essential nodejs npm git -y

RUN mkdir /root/work

WORKDIR /root/work