#!/bin/bash

# gets script directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# defines settings
PATH_CPPLINT="../external/styleguide/cpplint/cpplint.py"

# generates a list of OnSag files and runs cpplint
OPTIONS="--root="$DIR"/../include/ --extensions=h,cc --linelength=80 --filter=-legal/copyright"
DIRS_SEARCH="$DIR/../include/ $DIR/../src/"
FILES=$(find $DIRS_SEARCH -type f \( -name "*.cc" -o -name "*.h" \) -print)
$DIR/$PATH_CPPLINT $OPTIONS $FILES

# generates a list of AppCommon files and runs cpplint
OPTIONS="--root="$DIR"/../external/AppCommon/include/ --extensions=h,cc --linelength=80 --filter=-legal/copyright"
DIRS_SEARCH="$DIR/../external/AppCommon/include/ $DIR/../external/AppCommon/src/"
FILES=$(find $DIRS_SEARCH -type f \( -name "*.cc" -o -name "*.h" \) -print)
$DIR/$PATH_CPPLINT $OPTIONS $FILES
