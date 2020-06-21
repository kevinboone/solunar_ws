FROM alpine:3.12

#RUN apk add build-base
RUN addgroup -g 1000 mygroup && \
    mkdir /myuser && \
    adduser -G mygroup -u 1000 -h /myuser -D myuser && \
    chown -R myuser:mygroup /myuser && \
    apk add git build-base libmicrohttpd-dev tzdata zlib-dev

WORKDIR /myuser

USER myuser

RUN git clone https://github.com/kevinboone/solunar_ws.git && \
    make -C /myuser/solunar_ws

CMD ["/bin/sh"]

FROM alpine:3.12

RUN addgroup -g 1000 mygroup && \
    mkdir /myuser && \
    adduser -G mygroup -u 1000 -h /myuser -D myuser && \
    chown -R myuser:mygroup /myuser && \
    apk add libmicrohttpd-dev tzdata 

COPY --from=0 /myuser/solunar_ws/solunar_ws /

USER myuser

CMD ["/solunar_ws"]


