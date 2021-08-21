FROM debian:buster-slim
WORKDIR /profiler
COPY . .
RUN apt-get update
RUN apt-get install -y gcc make libmbedtls-dev
RUN make
EXPOSE 8000
CMD [ "/profiler/intra_profiler" ]
