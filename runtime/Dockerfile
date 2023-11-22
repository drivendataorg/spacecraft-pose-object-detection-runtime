FROM continuumio/miniconda3:23.10.0-1

# install OS dependencies
USER root
ENV DEBIAN_FRONTEND=noninteractive LANG=C.UTF-8 LC_ALL=C.UTF-8
RUN apt update --fix-missing && \
    apt install -y --no-install-recommends \
    software-properties-common pkg-config build-essential unzip git gcc g++ && \
    apt autoremove -y && \
    apt clean && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /usr/local/src/*

# create a limited user
ARG NEW_USER=appuser
RUN groupadd -g 999 $NEW_USER && \
    useradd -ms /bin/bash -r -u 1000 -g $NEW_USER $NEW_USER

# create the working directory with the entrypoint
RUN mkdir /submission && \
    chown -R $NEW_USER /submission
RUN mkdir /data && \
    chown -R $NEW_USER /data
USER $NEW_USER

# create conda environment
COPY environment.yml /tmp/
RUN conda env create -n condaenv -f /tmp/environment.yml && \
    conda clean --all --yes

# copy the tests into the container
COPY ./tests /tests

# install the entrypoint as root
USER root
COPY ./entrypoint.sh /entrypoint.sh
COPY ./validate.py /validate.py
RUN chmod +x /entrypoint.sh
USER $NEW_USER

# Execute the entrypoint.sh script inside the container when we do docker run
WORKDIR /submission
CMD ["/bin/bash", "/entrypoint.sh"]
