FROM ubuntu:24.04

#### INSTALL OPENMPI AND DEPENDENCIES ####
RUN apt-get --yes -qq update \
  && apt-get --yes -qq upgrade \
  && apt-get --yes -qq install \
  build-essential \
  bzip2 \
  cmake \
  cpio \
  curl \
  g++ \
  gcc \
  gfortran \
  git \
  gosu \
  libblas-dev \
  liblapack-dev \
  libopenmpi-dev \
  nano \
  openmpi-bin \
  openmpi-common \
  openmpi-doc \
  python3-dev \
  python3-pip \
  virtualenv \
  wget \
  zlib1g-dev \
  vim       \
  htop      \
  && apt-get --yes -qq clean \
  && rm -rf /var/lib/apt/lists/*

#### ADD DEFAULT USER ####
ARG USER=mpi
ENV USER ${USER}
RUN adduser --disabled-password ${USER}

ENV USER_HOME /home/${USER}
RUN chown -R ${USER}:${USER} ${USER_HOME}

#### CREATE WORKING DIRECTORY FOR USER ####
ARG WORKDIR=/project
ENV WORKDIR ${WORKDIR}
RUN mkdir ${WORKDIR}
RUN chown -R ${USER}:${USER} ${WORKDIR}

WORKDIR ${WORKDIR}
USER ${USER}
