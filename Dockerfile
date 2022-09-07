FROM cm0x4d/ubuntu-qt5-cmake
RUN mkdir -p /root/network
WORKDIR /root/network

COPY . network
WORKDIR /root/network/network
RUN mkdir cmake-build-debug
WORKDIR /root/network/network/cmake-build
RUN cmake ..
RUN make
