BUILD=../build
BIN=${BUILD}/zns_bench
DEV=nvme0n1

sudo nvme zns reset-zone /dev/$DEV -a

sudo ${BIN} \
  --dev=/dev/${DEV} \
  --bench=readrandom \
  --bs=$((8 * 1024 * 1024)) \
  --threads=1 \
  --duration=60
