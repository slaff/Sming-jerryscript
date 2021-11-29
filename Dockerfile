FROM ubuntu

RUN apt-get -y update -q \
    && DEBIAN_FRONTEND=noninteractive \
    TZ=Europe/London \
    apt-get -q -y install \
    tzdata \
    cmake \
    ninja-build \
    clang \
    default-jre \
    git-core \
    python3 \
    python3-pip \
    sudo && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN pip install pylint==1.6.5

ENV SDK_VERSION latest
ENV EMSDK /emsdk

# [Activate last working version ]
RUN git clone --depth 1 https://github.com/emscripten-core/emsdk.git $EMSDK && \
    $EMSDK/emsdk install $SDK_VERSION && \
    $EMSDK/emsdk activate $SDK_VERSION

SHELL ["/bin/bash", "-c"]

ENV JERRY_HOME /jerryscript

ARG JERRY_REPO=https://github.com/slaff/Sming-jerryscript
ARG JERRY_BRANCH=master

RUN git clone --depth 1 -b $JERRY_BRANCH $JERRY_REPO $JERRY_HOME --recurse-submodules

WORKDIR $JERRY_HOME

RUN source $EMSDK/emsdk_env.sh && \
    make -C emscripten-snapshot-compiler

# Installing WebAssembly Toolkit
RUN cd / && \
    git clone --recursive https://github.com/WebAssembly/wabt && \
    make -C wabt -j$(nproc)

ENV PATH=$PATH:/wabt/bin

ENTRYPOINT source $EMSDK/emsdk_env.sh && /bin/bash
CMD ["node","$JERRY_HOME/emscripten-snapshot-compiler/build/bin/minimal/jsc.js"]
