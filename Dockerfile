FROM ubuntu:18.04

RUN apt-get update && apt install -y gcc build-essential

RUN mkdir -p /var/www/app
# VOLUME ["/var/www/app"]
# COPY ./src /var/www/app

EXPOSE 3490

# docker run -ti -p 3490:3490 -v ${PWD}/src:/var/www/app test bash