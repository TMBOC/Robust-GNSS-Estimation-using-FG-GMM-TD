#!/bin/bash

CURRDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd && cd .. )"
DIR="$(dirname "$CURRDIR")"

TDIR="$DIR/test"
DDIR="$DIR/data"
BDIR="$DIR/examples/build"


D_L1_16bit_GPS_FDCS="$DDIR/gtsam/L1_16bit_GPS_FDCS.gtsam"
D_L1_16bit_GPS_FDPB="$DDIR/gtsam/L1_16bit_GPS_FDPB.gtsam"
D_L1_16bit_GPS_TDCS="$DDIR/gtsam/L1_16bit_GPS_TDCS.gtsam"
D_L1_16bit_GPS_TDPB="$DDIR/gtsam/L1_16bit_GPS_TDPB.gtsam"
D_L1_16bit_GPS_NoMit="$DDIR/gtsam/L1_16bit_GPS_NoMit.gtsam"
D_L1_8Bit_GPS_FDCS="$DDIR/gtsam/L1_8Bit_GPS_FDCS.gtsam"
D_L1_8Bit_GPS_FDPB="$DDIR/gtsam/L1_8Bit_GPS_FDPB.gtsam"
D_L1_8Bit_GPS_TDCS="$DDIR/gtsam/L1_8Bit_GPS_TDCS.gtsam"
D_L1_8Bit_GPS_TDPB="$DDIR/gtsam/L1_8Bit_GPS_TDPB.gtsam"
D_L1_8Bit_GPS_NoMit="$DDIR/gtsam/L1_8Bit_GPS_NoMit.gtsam"
cd "$TDIR"


#######################
#### RUN ALL TESTS ####
#######################


#######################
## L2
#######################
rm -rf "$TDIR/16bit/l2" "$TDIR/8Bit/l2"

# echo "running L2 1/6"
# "$BDIR/test_gnss_bce" -i "$D_LQ_1" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/lq/l2/01/" > /dev/null

# echo "running L2 2/6"
# "$BDIR/test_gnss_bce" -i "$D_HQ_1" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/hq/l2/01/" > /dev/null

echo "running L2 1/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_FDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/16bit/l2/FDCS/" > /dev/null

echo "running L2 2/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_FDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/16bit/l2/FDPB/" > /dev/null

echo "running L2 3/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_TDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/16bit/l2/TDCS/" > /dev/null

echo "running L2 4/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_TDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/16bit/l2/TDPB/" > /dev/null

echo "running L2 5/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_NoMit" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/16bit/l2/NoMit/" > /dev/null

echo "running L2 6/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_FDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/8Bit/l2/FDCS/" > /dev/null

echo "running L2 7/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_FDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/8Bit/l2/FDPB/" > /dev/null

echo "running L2 8/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_TDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/8Bit/l2/TDCS/" > /dev/null

echo "running L2 9/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_TDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/8Bit/l2/TDPB/" > /dev/null

echo "running L2 10/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_NoMit" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 0 --dir "$TDIR/8Bit/l2/NoMit/" > /dev/null


#######################
## DCS
#######################
rm -rf "$TDIR/16bit/dcs" "$TDIR/8Bit/dcs"

# echo "running DCS 1/6"
# "$BDIR/test_gnss_dcs" -i "$D_LQ_1" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/lq/dcs/01/" > /dev/null

# echo "running DCS 2/6"
# "$BDIR/test_gnss_dcs" -i "$D_HQ_1" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/hq/dcs/01/" > /dev/null

echo "running dcs 1/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_16bit_GPS_FDCS" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/16bit/dcs/FDCS/" > /dev/null

echo "running dcs 2/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_16bit_GPS_FDPB" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/16bit/dcs/FDPB/" > /dev/null

echo "running dcs 3/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_16bit_GPS_TDCS" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/16bit/dcs/TDCS/" > /dev/null

echo "running dcs 4/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_16bit_GPS_TDPB" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/16bit/dcs/TDPB/" > /dev/null

echo "running dcs 5/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_16bit_GPS_NoMit" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/16bit/dcs/NoMit/" > /dev/null

echo "running dcs 6/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_8Bit_GPS_FDCS" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/8Bit/dcs/FDCS/" > /dev/null

echo "running dcs 7/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_8Bit_GPS_FDPB" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/8Bit/dcs/FDPB/" > /dev/null

echo "running dcs 8/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_8Bit_GPS_TDCS" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/8Bit/dcs/TDCS/" > /dev/null

echo "running dcs 9/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_8Bit_GPS_TDPB" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/8Bit/dcs/TDPB/" > /dev/null

echo "running dcs 10/10"
"$BDIR/test_gnss_dcs" -i "$D_L1_8Bit_GPS_NoMit" --writeECEF --writeENU --writeBias --kernelWidth 2.5 --dir "$TDIR/8Bit/dcs/NoMit/" > /dev/null


#######################
## Max-Mix
#######################
rm -rf "$TDIR/16bit/mm" "$TDIR/8Bit/mm"

# echo "running MM 1/6"
# "$BDIR/test_gnss_maxmix" -i "$D_LQ_1" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/lq/mm/01/" > /dev/null
 
# echo "running MM 2/6"
# "$BDIR/test_gnss_maxmix" -i "$D_HQ_1" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/hq/mm/01/" > /dev/null

echo "running mm 1/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_16bit_GPS_FDCS" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/16bit/mm/FDCS/" > /dev/null

echo "running mm 2/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_16bit_GPS_FDPB" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/16bit/mm/FDPB/" > /dev/null

echo "running mm 3/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_16bit_GPS_TDCS" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/16bit/mm/TDCS/" > /dev/null

echo "running mm 4/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_16bit_GPS_TDPB" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/16bit/mm/TDPB/" > /dev/null

echo "running mm 5/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_16bit_GPS_NoMit" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/16bit/mm/NoMit/" > /dev/null

echo "running mm 6/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_8Bit_GPS_FDCS" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/8Bit/mm/FDCS/" > /dev/null

echo "running mm 7/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_8Bit_GPS_FDPB" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/8Bit/mm/FDPB/" > /dev/null

echo "running mm 8/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_8Bit_GPS_TDCS" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/8Bit/mm/TDCS/" > /dev/null

echo "running mm 9/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_8Bit_GPS_TDPB" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/8Bit/mm/TDPB/" > /dev/null

echo "running mm 10/10"
"$BDIR/test_gnss_maxmix" -i "$D_L1_8Bit_GPS_NoMit" --writeECEF --writeENU --writeBias --mixWeight 1e-6 --dir "$TDIR/8Bit/mm/NoMit/" > /dev/null


#######################
## BCE
#######################
rm -rf "$TDIR/16bit/bce" "$TDIR/8Bit/bce"

# echo "running BCE 1/6"
# "$BDIR/test_gnss_bce" -i "$D_LQ_1" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/lq/bce/01/" > /dev/null

# echo "running BCE 2/6"
# "$BDIR/test_gnss_bce" -i "$D_HQ_1" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/hq/bce/01/" > /dev/null

echo "running bce 1/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_FDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/16bit/bce/FDCS/" > /dev/null

echo "running bce 2/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_FDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/16bit/bce/FDPB/" > /dev/null

echo "running bce 3/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_TDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/16bit/bce/TDCS/" > /dev/null

echo "running bce 4/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_TDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/16bit/bce/TDPB/" > /dev/null

echo "running bce 5/10"
"$BDIR/test_gnss_bce" -i "$D_L1_16bit_GPS_NoMit" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/16bit/bce/NoMit/" > /dev/null

echo "running bce 6/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_FDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/8Bit/bce/FDCS/" > /dev/null

echo "running bce 7/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_FDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/8Bit/bce/FDPB/" > /dev/null

echo "running bce 8/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_TDCS" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/8Bit/bce/TDCS/" > /dev/null

echo "running bce 9/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_TDPB" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/8Bit/bce/TDPB/" > /dev/null

echo "running bce 10/10"
"$BDIR/test_gnss_bce" -i "$D_L1_8Bit_GPS_NoMit" --writeECEF --writeENU --writeBias --phaseScale 100 --robustIter 100 --dir "$TDIR/8Bit/bce/NoMit/" > /dev/null


clear
echo -e "\n\n\n\n ----------------------------------------------- \n"
echo -e " run done. :-)  All results were written to ../test"
echo -e  "\n ----------------------------------------------- \n\n\n"
