ARG BASE_IMAGE_VERSION=cpp17_ROOT-v6-25-01_Geant4-v11.0.0_Garfield-4.0

FROM ghcr.io/lobis/root-geant4-garfield:${BASE_IMAGE_VERSION}

# to be called from the root directory
ADD . /source

ENV INSTALL_PREFIX=/install

RUN mkdir -p /source/build && cd /source/build && \
    cmake .. -DTEST=OFF -DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} && \
    make install && \
    rm -rf /source/build

RUN echo "source ${INSTALL_PREFIX}/env.sh" >> ~/.bashrc

WORKDIR /install/examples/simulation

CMD ["/bin/bash"]
