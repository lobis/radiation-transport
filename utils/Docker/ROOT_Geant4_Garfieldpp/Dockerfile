FROM ubuntu:latest

LABEL maintainer.name="Luis Obis"
LABEL maintainer.email="luis.antonio.obis@gmail.com"

LABEL org.opencontainers.image.source=https://github.com/lobis/radiation-transport.git

ARG APPS_DIR=/opt

RUN DEBIAN_FRONTEND="noninteractive" && \
    apt-get update && \
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

ENV ROOTSYS=$APPS_DIR/root/install

RUN git clone https://github.com/root-project/root.git --branch=${ROOT_VERSION} --depth 1 $APPS_DIR/root/source && \
    mkdir -p $APPS_DIR/root/build &&  cd $APPS_DIR/root/build && \
    cmake $APPS_DIR/root/source -DCMAKE_INSTALL_PREFIX=$ROOTSYS -DGDML=ON -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD && \
    make -j$(nproc) install && \
    rm -rf $APPS_DIR/root/build $APPS_DIR/root/source && \
    echo "source $ROOTSYS/bin/thisroot.sh" >> ~/.bashrc

ENV PATH $ROOTSYS/bin:$PATH
ENV LD_LIBRARY_PATH $ROOTSYS/lib:$LD_LIBRARY_PATH
ENV PYTHONPATH $ROOTSYS/lib:$PYTHONPATH

# GEANT4
ARG GEANT4_VERSION
RUN echo GEANT4_VERSION: $GEANT4_VERSION

RUN git clone https://github.com/Geant4/geant4.git --branch=${GEANT4_VERSION} --depth 1 $APPS_DIR/geant4/source && \
    mkdir -p $APPS_DIR/geant4/build &&  cd $APPS_DIR/geant4/build && \
    cmake ../source/ -DCMAKE_INSTALL_PREFIX=$APPS_DIR/geant4/install -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD -DGEANT4_BUILD_CXXSTD=$CMAKE_CXX_STANDARD \
    -DGEANT4_BUILD_MULTITHREADED=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_RAYTRACER_X11=ON \
    -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_GDML=ON && \
    make -j$(nproc) install && \
    rm -rf $APPS_DIR/geant4/build $APPS_DIR/geant4/source && \
    echo "source $APPS_DIR/geant4/install/bin/geant4.sh" >> ~/.bashrc

ENV PATH $APPS_DIR/geant4/install/bin:$PATH
ENV LD_LIBRARY_PATH $APPS_DIR/geant4/install/lib:$LD_LIBRARY_PATH

# GARFIELDPP
ARG GARFIELDPP_VERSION=4.0
RUN echo GARFIELDPP_VERSION: GARFIELDPP_VERSION

RUN git clone https://gitlab.cern.ch/garfield/garfieldpp.git --branch=${GARFIELDPP_VERSION} --depth 1 $APPS_DIR/garfieldpp/source && \
    mkdir -p $APPS_DIR/garfieldpp/build &&  cd $APPS_DIR/garfieldpp/build && \
    cmake ../source/ -DCMAKE_INSTALL_PREFIX=$APPS_DIR/garfieldpp/install \
    -DWITH_EXAMPLES=OFF -DCMAKE_CXX_STANDARD=$CMAKE_CXX_STANDARD && \
    make -j$(nproc) install && \
    rm -rf $APPS_DIR/garfieldpp/build $APPS_DIR/garfieldpp/source && \
    echo "source $APPS_DIR/garfieldpp/install/share/Garfield/setupGarfield.sh" >> ~/.bashrc

ENV LD_LIBRARY_PATH $APPS_DIR/garfieldpp/install/lib:$LD_LIBRARY_PATH

WORKDIR /

CMD ["/bin/bash"]
