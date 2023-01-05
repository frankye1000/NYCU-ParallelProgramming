set -e

input='small_1.txt'

mkdir -p profiles

make seq_main omp_main

echo "--------------------------------------------------------------------------------"
echo "--------------------------------------------------------------------------------"

# TODO: Add quotes around ${input} so that spaces in the filename don't break things

for k in 2 4 8 16 32 64 128; do
    seqTime=$(./seq_main.exe -o -n $k -i Image_data/${input} | grep 'Computation' | awk '{print $4}')
    gprof ./seq_main.exe > profiles/seq-profile-$k.txt
    #mv Image_data/${input}.cluster_centres Image_data/${input}-$k.cluster_centres
    #mv Image_data/${input}.membership Image_data/${input}-$k.membership

    ompTime=$(./omp_main.exe -o -n $k -i Image_data/${input} | grep 'Computation' | awk '{print $4}')
    gprof ./omp_main.exe > profiles/omp-profile-$k.txt

    speedup=$(echo "scale=1; ${seqTime} / ${ompTime}" | bc)
    echo "k = $(printf "%3d" $k)  seqTime = ${seqTime}s  ompTime = ${ompTime}s  speedup = ${speedup}x"
done
