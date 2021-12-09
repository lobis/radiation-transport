ARG CMAKE_CXX_STANDARD
ARG ROOT_VERSION
ARG GEANT4_VERSION

FROM ghcr.io/lobis/root-geant4-garfieldpp:cxx${CMAKE_CXX_STANDARD}_ROOT${ROOT_VERSION}_Geant4${GEANT4_VERSION}

SHELL ["/bin/bash", "-c"]

RUN DEBIAN_FRONTEND="noninteractive" apt-get update && apt-get -y install tzdata

RUN apt-get update \
    && apt-get install -y  \
    ssh \
    xauth \
    xorg \
    build-essential \
    gcc \
    g++ \
    gdb \
    clang \
    cmake \
    rsync \
    tar \
    python \
    nano \
    x11-apps \
    nodejs \
    && apt-get clean

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
    ) > /etc/ssh/sshd_config_test_clion \
    && mkdir /run/sshd

RUN ( \
    echo 'ForwardAgent yes'; \
    echo 'ForwardX11 yes'; \
    echo 'ForwardX11Trusted yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Port 22'; \
    echo 'Protocol 2'; \
    echo 'GSSAPIDelegateCredentials no'; \
    echo 'XauthLocation /usr/bin/xauth'; \
    ) >> /etc/ssh/ssh_config

RUN useradd -m user && yes password | passwd user

# Jupyter
RUN pip3 -q install pip --upgrade && \
    pip3 list --outdated --format=freeze | grep -v '^\-e' | cut -d = -f 1 | xargs -n1 pip3 -q install && \
    pip3 -q install uproot awkward matplotlib scipy pandas mplhep boost_histogram jupyterlab

RUN cp /root/.bashrc /home/user/.bashrc

RUN usermod -s /bin/bash user

USER user

WORKDIR /home/user

RUN echo 'export DISPLAY=localhost:0' >> /home/user/.bashrc

RUN mkdir -p /home/user/Notebooks
RUN echo "alias start-jupyter='cd /home/user/Notebooks && jupyter lab --ip=0.0.0.0 --no-browser'" >> /home/user/.bashrc

# Add Tini
ENV TINI_VERSION v0.19.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--"]

EXPOSE 2222
EXPOSE 8888

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]
