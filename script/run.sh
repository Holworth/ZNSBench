BUILD=../build
BIN=${BUILD}/zns_bench
DEV=nvme0n1

sudo nvme zns reset-zone /dev/$DEV -a

sudo gdb --args ${BIN} \
  --dev=/dev/${DEV} \
  --bench=writeseq \
  --bs=$((4 * 1024 * 1024)) \
  --threads=1 \
  --duration=60
