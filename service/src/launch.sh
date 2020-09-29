#!/bin/bash -e

./launch_bin.sh cpu/cpu_server &
./launch_bin.sh gpu/gpu_server &
#./launch_bin.sh romloader/romloader_server &
./launch_bin.sh input/input_server &
./launch_bin.sh logs/logs_server &
./launch_bin.sh pyromloader/romloader.py &

cd firewall
while true;
do
	ruby ./firewall.rb ./metacfg.json ./firewall_rules.json
done
