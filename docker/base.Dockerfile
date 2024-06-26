ARG BASE_IMAGE='ubuntu:22.04'
FROM ${BASE_IMAGE} as base

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
