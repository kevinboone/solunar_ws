FROM alpine:3.12

RUN addgroup -g 1000 mygroup && \
    mkdir /myuser && \
    adduser -G mygroup -u 1000 -h /myuser -D myuser && \
    chown -R myuser:mygroup /myuser && \
    apk add git build-base tzdata zlib-dev && \
    wget https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-latest.tar.gz && \
    tar xfvz libmicrohttpd-latest.tar.gz && \
    (cd libmi*; ./configure; make install) && \
    git clone https://github.com/kevinboone/solunar_ws.git && \
    make -C solunar_ws

WORKDIR /myuser

USER myuser

CMD ["/bin/sh"]

FROM alpine:3.12

RUN addgroup -g 1000 mygroup && \
    mkdir /myuser && \
    adduser -G mygroup -u 1000 -h /myuser -D myuser && \
    chown -R myuser:mygroup /myuser && \
    apk add tzdata 

COPY --from=0 /solunar_ws/solunar_ws /
COPY --from=0 /usr/local/lib/libmicrohttpd.so.12 /usr/local/lib

USER myuser

CMD ["/solunar_ws"]


