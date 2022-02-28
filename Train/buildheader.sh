#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
FILES="*.html *.css *.js"
OUTPUT=$DIR/html.h

cd $DIR/html

cat <<EOF >$OUTPUT
#ifndef HTML_H
#define HTML_H

/*
  NOTE: This file is generated, do not edit directly
  
  Edit files in html/ and run buildheader.sh to update
*/

EOF

for I in $FILES; do
    VARNAME=${I//[.\/]/_}
    echo -n const char $VARNAME[] = PROGMEM {\" >>$OUTPUT
    cat $I | sed -e 's/"/\\"/g' -e "s/\'/\\\\n\\\\/g" >>$OUTPUT
    echo '"};' >>$OUTPUT
done

cat <<EOF >>$OUTPUT

const char* mapFile(String file) {
EOF

for I in $FILES; do
    VARNAME=${I//[.\/]/_}
    cat <<EOF >>$OUTPUT
  if(file=="/$I") return $VARNAME;
EOF

done

cat <<EOF >>$OUTPUT
  return NULL;
}

#endif
EOF
