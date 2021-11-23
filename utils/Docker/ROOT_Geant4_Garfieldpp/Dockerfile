FROM ubuntu:latest

LABEL maintainer.name="Luis Obis"
LABEL maintainer.email="luis.antonio.obis@gmail.com"

ENV APPS_DIR=/opt

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y \
    apt-utils binutils build-essential ca-certificates cmake curl davix-dev dcap-dev dpkg-dev fonts-freefont-ttf g++ gcc gfortran \
    git libafterimage-dev libcfitsio-dev libexpat-dev libfcgi-dev libfftw3-dev libfreetype6-dev libftgl-dev libgfal2-dev libgif-dev \
    libgl2ps-dev libglew-dev libglu-dev libgraphviz-dev libgsl-dev libjpeg-dev liblz4-dev liblzma-dev libmpc-dev libmysqlclient-dev  \
    libpcre++-dev libpng-dev libpq-dev libpythia8-dev libspdlog-dev libsqlite3-dev libssl-dev libtbb-dev libtiff-dev libx11-dev \
    libxerces-c-dev libxext-dev libxft-dev libxml2-dev libxmu-dev libxpm-dev libxxhash-dev libz-dev libzstd-dev locales make openssl \
    python3-dev python3-numpy qt5-default software-properties-common srm-ifce-dev unixodbc-dev wget \
    libpq-dev postgresql-server-dev-all libboost-all-dev && \
    apt-get autoremove -y && \
    apt-get clean -y && \
    rm -rf /var/cache/apt/archives/* && \
    rm -rf /var/lib/apt/lists/*

ARG CMAKE_CXX_STANDARD
RUN echo CMAKE_CXX_STANDARD: $CMAKE_CXX_STANDARD

# ROOT
ARG ROOT_VERSION
RUN echo ROOT_VERSION: $ROOT_VERSION
ENV ROOT_DIR=$APPS_DIR/root

ENV ROOTSYS=$ROOT_DIR/install

RUN git clone https://github.com/root-project/root.git --branch=${ROOT_VERSION} --depth 1 $ROOT_DIR/source && \
    mkdir -p $ROOT_DIR/build &&  cd $ROOT_DIR/build && \
    cmake $ROOT_DIR/source -DCMAKE_INSTALL_PREFIX=$ROOTSYS -DGDML=ON -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD && \
    make -j$(nproc) install && \
    rm -rf $ROOT_DIR/build $ROOT_DIR/source && \
    echo "source $ROOTSYS/bin/thisroot.sh" >> ~/.bashrc

ENV PATH $ROOTSYS/bin:$PATH
ENV PYTHONPATH $ROOTSYS/lib:$PYTHONPATH
ENV CLING_STANDARD_PCH none

# GEANT4
ARG GEANT4_VERSION
RUN echo GEANT4_VERSION: $GEANT4_VERSION
ENV GEANT4_DIR=$APPS_DIR/geant4

RUN git clone https://github.com/Geant4/geant4.git --branch=${GEANT4_VERSION} --depth 1 $GEANT4_DIR/source && \
    mkdir -p $GEANT4_DIR/build &&  cd $GEANT4_DIR/build && \
    cmake ../source/ -DCMAKE_INSTALL_PREFIX=$GEANT4_DIR/install -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD -DGEANT4_BUILD_CXXSTD=$CMAKE_CXX_STANDARD \
    -DGEANT4_BUILD_MULTITHREADED=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_RAYTRACER_X11=ON \
    -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_GDML=ON && \
    make -j$(nproc) install && \
    rm -rf $GEANT4_DIR/build $GEANT4_DIR/source && \
    echo "source $GEANT4_DIR/install/bin/geant4.sh" >> ~/.bashrc

ENV PATH $GEANT4_DIR/install/bin:$PATH

# GARFIELDPP
ARG GARFIELDPP_VERSION=4.0
ENV GARFIELDPP_DIR=$APPS_DIR/garfieldpp

RUN git clone https://gitlab.cern.ch/garfield/garfieldpp.git --branch=${GARFIELDPP_VERSION} --depth 1 $GARFIELDPP_DIR/source && \
    mkdir -p $GARFIELDPP_DIR/build &&  cd $GARFIELDPP_DIR/build && \
    cmake ../source/ -DCMAKE_INSTALL_PREFIX=$GARFIELDPP_DIR/install \
    -DWITH_EXAMPLES=OFF -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD && \
    make -j$(nproc) install && \
    rm -rf $GARFIELDPP_DIR/build $GARFIELDPP_DIR/source && \
    echo "source $GARFIELDPP_DIR/install/share/Garfield/setupGarfield.sh" >> ~/.bashrc

WORKDIR /

CMD ["/bin/bash"]