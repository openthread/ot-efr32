ARG BASE_IMAGE='siliconlabsinc/ot-efr32-dev:base'

FROM ${BASE_IMAGE} AS ot-efr32-dev

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
