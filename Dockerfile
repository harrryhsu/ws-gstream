FROM ubuntu:20.04

RUN apt update && DEBIAN_FRONTEND=noninteractive apt install build-essential nodejs npm git -y

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
	libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good\
	gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-doc\
	gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3\
	gstreamer1.0-qt5 gstreamer1.0-pulseaudio gstreamer1.0-rtsp ffmpeg cmake

RUN mkdir /build && cd /build &&\
	git clone https://github.com/warmcat/libwebsockets.git && \
	cd libwebsockets && \
	cmake . && make && make install &&\
	rm -rf /build

RUN mkdir /root/work

WORKDIR /root/work
