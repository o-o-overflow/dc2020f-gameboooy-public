FROM ubuntu:18.04
ENV TERM linux
RUN apt-get -qq update && apt-get install -qq --no-install-recommends ruby && gem install timeout

COPY check1.rb /check1.rb
COPY check1 /check1
COPY opus5.gb /opus5.gb
COPY c1.bin /c1.bin
COPY c2.bin /c2.bin

RUN chmod +x /check1.rb
RUN chmod +x /check1
