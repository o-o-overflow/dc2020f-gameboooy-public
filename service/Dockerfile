# from ubuntu:20.04
# 
# # example:
# run apt-get -qq update && DEBIAN_FRONTEND=noninteractive apt-get install -qq --no-install-recommends clang cmake clang-10 build-essential git
# # python3-pip, etc.
# 
# # Note: anything that goes in public_files must be pre-built and checked in git
# copy /src /src
# run cd /src && rm -rf rpclib/build && ./build_rpclib.sh
# run cd /src && make

from ubuntu:20.04
run apt-get -qq update && apt-get install -qq --no-install-recommends socat ruby2.7-dev build-essential clang python3 python3-pip python3-dev
run gem install msgpack minitcp gserver pry

run mkdir /cpu /gpu /romloader /input /firewall /logs /pyromloader
copy src/cpu/cpu_server /cpu/cpu_server
copy src/gpu/gpu_server /gpu/gpu_server
#copy src/romloader/romloader_server /romloader/romloader_server
copy src/input/input_server /input/input_server
copy src/logs/logs_server /logs/logs_server
copy src/firewall/firewall.rb /firewall/firewall.rb
copy src/firewall/cpu.json /firewall/cpu.json
copy src/firewall/gpu.json /firewall/gpu.json
copy src/firewall/input.json /firewall/input.json
copy src/firewall/romloader.json /firewall/romloader.json
copy src/firewall/logs.json /firewall/logs.json
copy src/firewall/metacfg.json /firewall/metacfg.json
copy src/firewall/firewall_rules.json /firewall/firewall_rules.json
copy src/launch.sh /launch.sh
copy src/launch_bin.sh /launch_bin.sh
COPY src/pyromloader/romloader.py /pyromloader/romloader.py
COPY src/pyromloader/requirements.txt /pyromloader/requirements.txt
RUN pip3 install -r /pyromloader/requirements.txt

# The flagbot will periodically echo the flag > this file (flag_path in info.yaml)
run touch /flag && chmod 644 /flag && echo "OOO{TESTFLAG}" > /flag

# TODO: If possible, change this to be unique (here and in xinetd)
#       Otherwise, specify a unique game_port in info.yml
expose 8069


cmd chmod go-rwx /proc && /launch.sh
# ^^ Assuming that ps would be too revealing.
#    AFAIK, this also disables the built-in printf(%n) protection. Alternatively:
#cmd ["/usr/sbin/xinetd", "-dontfork", "-f", "/service.conf"]

# TODO: -filelog /dev/stderr
