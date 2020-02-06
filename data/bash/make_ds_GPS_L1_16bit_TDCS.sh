#!/bin/bash


##
echo "Making data for GPS_L1_16bit_TDCS"
rnx_2_gtsam --obs L1_16bit_GPS_TDCS.rnx --sp3 jpl20705.sp3 --brdc_nav hour2560.19n --break_thresh 5.5 --break_window 1500 > L1_16bit_GPS_TDCS.gtsam


echo "finished with data for GPS_L1_16bit_TDCS"
