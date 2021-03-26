#!/bin/bash

scp firmware/build/*.bin firmware/build/bootloader/bootloader.bin firmware/build/ota_data_initial.bin firmware/build/partition_table/partition-table.bin $1;
