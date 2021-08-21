FROM debian:buster-slim
WORKDIR /profiler
COPY . .
RUN apt-get update; \
		apt-get install -y \
		make \
		gcc \
		dumb-init \
		libmongoc-1.0-0 \
		libmongoc-dev \
		libmbedtls-dev
EXPOSE 80

ENTRYPOINT [ "/usr/bin/dumb-init", "make", "run" ]
