FROM ghcr.io/lobis/root-geant4-garfieldpp:cxx17_ROOTv6-25-01_Geant4v10.7.3

# context should be repository root directory

SHELL ["/bin/bash", "-c"]

# Jupyter

RUN source ~/.bashrc && \
    pip3 -q install pip --upgrade && \
    pip3 -q install uproot awkward matplotlib pandas mplhep boost_histogram jupyterlab

# Add Tini
ENV TINI_VERSION v0.19.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--"]

# Install repository

ADD . /radiation-transport

WORKDIR /radiation-transport

RUN mkdir build && \
    cd build && \
    source entry-point.sh && \
    cmake .. -DCMAKE_INSTALL_PREFIX=/radiation-transport/install && \
    make install

RUN echo "source /radiation-transport/install/env.sh" >> ~/.bashrc

WORKDIR /

CMD ["/bin/bash"]
