ARG BASE_IMAGE='ubuntu:22.04'

FROM ${BASE_IMAGE} AS base

ENV TZ="America/New_York"
ENV repo_dir="/ot-efr32"
WORKDIR ${repo_dir}

# Install packages
RUN apt-get update && \
      apt-get -y install --no-install-recommends \
      sudo \
      tzdata \
      && rm -rf /var/lib/apt/lists/*

# Copy scripts
COPY  ./script/bootstrap \
      ./script/

# Install system packages and ARM toolchain
RUN ./script/bootstrap packages && rm -rf /var/lib/apt/lists/*
RUN ./script/bootstrap arm_toolchain

# Install Python packages
COPY ./requirements.txt .
RUN ./script/bootstrap python

# ==============================================================================

# Label the build date before downloading slc to force slc to always be downloaded during a docker build

FROM base AS ot-efr32-dev
ARG BUILD_DATE
LABEL build_date=${BUILD_DATE}

COPY  ./script/bootstrap_silabs \
      ./script/
ENV SLC_INSTALL_DIR=/opt/slc_cli
RUN mkdir ${SLC_INSTALL_DIR} && \
      ./script/bootstrap silabs


# Change workdir to root temporarily
WORKDIR /

# Clone repo for convenience
ARG REPO_URL="https://github.com/openthread/ot-efr32"
ENV repo_dir="/ot-efr32"
RUN rm -rf ${repo_dir} && git clone ${REPO_URL} ${repo_dir}

# Change workdir back to repo
WORKDIR ${repo_dir}
