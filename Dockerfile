FROM ubuntu:22.04

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
COPY ./openthread/script ./openthread/script
COPY  ./script/bootstrap \
      ./script/bootstrap_silabs \
      ./script/
COPY ./requirements.txt .

# Bootstrap
RUN ./script/bootstrap packages && rm -rf /var/lib/apt/lists/*

ENV SLC_INSTALL_DIR=/opt/slc_cli
RUN mkdir ${SLC_INSTALL_DIR} && \
      ./script/bootstrap silabs

RUN ./script/bootstrap openthread && rm -rf /var/lib/apt/lists/*

# Clone repo for convenience
ARG REPO_URL="https://github.com/openthread/ot-efr32"
WORKDIR /
RUN rm -rf ${repo_dir} && git clone ${REPO_URL} ${repo_dir}
WORKDIR ${repo_dir}
