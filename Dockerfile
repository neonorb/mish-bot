FROM reo7sp/tgbot-cpp
MAINTAINER Neon Orb <neonorb@neonorb.com>

WORKDIR /tmp/mishbot
COPY . .
RUN make
CMD build/mishbot
