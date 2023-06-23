#/bin/sh
set -e
directory=$1
tzif_list=$2

truncate -s 0 $tzif_list

for i in `find $directory -type f`; do
    header=`head -c 4 $i`
    if [ "$header" = "TZif" ]; then
        echo "$i" >> $tzif_list
    fi
done

