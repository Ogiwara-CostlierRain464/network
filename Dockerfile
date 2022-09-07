FROM cm0x4d/ubuntu-qt5-cmake
RUN mkdir -p /root/network
WORKDIR /root/network

COPY . network
RUN mkdir cmake-build-debug
WORKDIR /root/network/cmake-build
RUN cmake ..
RUN make
