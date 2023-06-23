#!/bin/sh
BINARY_LIST_TXT="$1"

# Fill text file with zone names
# Issue: Hyphens/underscores are not handled the same way Nintendo handles them
get_files_nx() {
    local target=$1
    find $target -maxdepth 1 -type f -not -regex '.*Factory' | sort
    local DIRS=`find $target -maxdepth 1 -type d | sort`
    for i in $DIRS; do
        if [ "`readlink -e $i`" = "`readlink -e $target`" ]; then
            continue
        fi
        get_files_nx $i
    done
}

get_files_nx . | cut -c3- > ${BINARY_LIST_TXT}

# Convert LF to CRLF
awk -v ORS='\r\n' 1 ${BINARY_LIST_TXT} > win
# Overwrite LF with CRLF conversion
mv win ${BINARY_LIST_TXT}
