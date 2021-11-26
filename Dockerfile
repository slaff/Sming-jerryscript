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

# [Activate last working version ]
RUN git clone https://github.com/emscripten-core/emsdk.git && \
    cd emsdk && \
    ./emsdk install $SDK_VERSION && \
    ./emsdk activate $SDK_VERSION && \
    /bin/bash -c "source ./emsdk_env.sh"

SHELL ["/bin/bash", "-c"]

ENV SMING_HOME /sming/Sming
ENV SMING_SOC host
ENV JERRY_HOME $SMING_HOME/Libraries/jerryscript

ARG JERRY_REPO=https://github.com/slaff/Sming-jerryscript
ARG JERRY_BRANCH=master

RUN git clone --depth 1 https://github.com/SmingHub/Sming /sming && \
    /sming/Tools/install.sh host && \
    rm -rf $JERRY_HOME && \
    git clone -b $JERRY_BRANCH $JERRY_REPO $JERRY_HOME && \
    touch $JERRY_HOME/.submodule && \
    ln -s $JERRY_HOME/jerryscript /jerryscript && \
    make -C $JERRY_HOME/samples/Basic_Jsvm ../../jerryscript/.submodule

WORKDIR $SMING_HOME/Libraries/jerryscript

ENV EMSCRIPTEN /emsdk/upstream/emscripten

RUN source /emsdk/emsdk_env.sh && \
    python3 /jerryscript/tools/build.py --emscripten-snapshot-compiler ON --verbose --profile=minimal

# Installing WebAssembly Toolkit
RUN cd / && \
    git clone --recursive https://github.com/WebAssembly/wabt && \
    make -C wabt -j$(nproc)

ENV PATH=$PATH:/wabt/bin

ENTRYPOINT source /emsdk/emsdk_env.sh && /bin/bash
CMD ["node","/jerryscript/build/bin/jsc.js"]
