#!/usr/bin/env bash

set -eu

serial_port="${1:-/dev/ttyACM0}"
firmware_path="build/fluxpad_midi.uf2"
mount_path="/mnt/usb"
boot_device=""
mounted_by_script=0

cleanup() {
    if [ "$mounted_by_script" -eq 1 ]; then
        sudo umount "$mount_path"
    fi
}

cmake --build build

python3 -c 'import serial, sys; p = serial.Serial(sys.argv[1], 1200); p.close()' "$serial_port"

for attempt in $(seq 1 50); do
    if [ -e /dev/disk/by-label/RPI-RP2 ]; then
        boot_device="$(readlink -f /dev/disk/by-label/RPI-RP2)"
        break
    else
        sleep 0.1
    fi
done

if [ -n "$boot_device" ]; then
    if mountpoint -q "$mount_path"; then
        printf '%s is already mounted; refusing to overwrite it.\n' "$mount_path"
        exit 1
    else
        sudo mount "$boot_device" "$mount_path"
        mounted_by_script=1
        trap cleanup EXIT
        sudo cp "$firmware_path" "$mount_path/"
        sync
        printf 'Flashed %s to %s.\n' "$firmware_path" "$boot_device"
    fi
else
    printf 'RP2040 boot drive RPI-RP2 did not appear.\n'
    exit 1
fi
