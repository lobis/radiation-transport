FROM ghcr.io/lobis/root-geant4-garfieldpp:cxx17_ROOTv6-25-01_Geant4v10.7.3

# context should be repository root directory

SHELL ["/bin/bash", "-c"]

# Install Anaconda3

RUN wget --quiet https://repo.anaconda.com/archive/Anaconda3-2021.11-Linux-x86_64.sh -O ~/anaconda.sh && \
    /bin/bash ~/anaconda.sh -b -p /conda && \
    rm ~/anaconda.sh && \
    ln -s /conda/etc/profile.d/conda.sh /etc/profile.d/conda.sh && \
    echo ". /conda/etc/profile.d/conda.sh" >> ~/.bashrc && \
    echo "conda activate base" >> ~/.bashrc \

# Jupyter

RUN source ~/.bashrc && \
    pip3 -q install pip --upgrade && \
    pip3 install uproot awkward jupyter

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
