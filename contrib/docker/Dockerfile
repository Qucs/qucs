FROM alpine:3.6

ARG QUCS_VERSION=${QUCS_VERSION:-"master"}
ARG ADMS_VERSSION=${ADMS_VERSSION:-"master"}
ARG ASCO_VERSSION=${ASCO_VERSSION:-"0.4.10"}

RUN set -xe \
    && apk add --update \

    && apk add --no-cache --virtual .deps \
           qt-x11 \
           icu-libs \
           ttf-dejavu \

    && apk add --no-cache --virtual .build-deps \
           build-base \
           curl \
           libtool \
           autoconf \
           automake \
           bison \
           flex \
           git \
           git-svn \
           perl-gd \
           perl-xml-libxml \
           qt-dev \
           gperf \
           icu-dev \

    # Build ADMS provides admsXml which is needed for compiling Qucs Verilog-a modules  
    && git clone --single-branch --depth=1 --branch=$ADMS_VERSSION https://github.com/Qucs/ADMS.git /tmp/src/adms \
    && cd /tmp/src/adms \
    && ./bootstrap.sh \
    && ./configure --disable-doc --prefix=/usr/local \
    && make \ 
    && make install \

    # Build ASCO is a circuit optimizer, used by Qucs
    && cd /tmp \
    && curl -SLO "https://10gbps-io.dl.sourceforge.net/project/asco/asco/$ASCO_VERSSION/ASCO-$ASCO_VERSSION.tar.gz" \		
    && tar -zxvf ASCO-$ASCO_VERSSION.tar.gz \		
    && cd ASCO-$ASCO_VERSSION \		
    && make \
    && cp asco /usr/local/bin/asco \

    # Build Qucs
    && git clone --single-branch --depth=1 --branch=$QUCS_VERSION https://github.com/Qucs/qucs.git /tmp/src/qucs \
    && cd /tmp/src/qucs \
    && ./bootstrap \
    && ./configure --disable-doc --prefix=/usr/local \
    && make \
    && make install \
    
    # Setup enveroument
    && adduser -D -h /home creator \
    && echo "export DISPLAY=unix:0.0" | tee -a /etc/profile.d/X11.sh \
	
    # Clean
    && rm -rf /tmp/* \
    && apk del --purge .build-deps

COPY entrypoint.sh /usr/local/bin/entrypoint

WORKDIR /home

ENTRYPOINT ["entrypoint"]
