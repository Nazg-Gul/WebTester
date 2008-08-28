#!/bin/sh

#
# Script to run solution at all tests and print out result
#
# Written (by Nazgul) under GPL. 03.10.2007
#

echo -ne "#!/bin/sh\n./check_entry.sh \"" > ./z_check.sh
cat tests.info >> ./z_check.sh
echo -n "\" " >> ./z_check.sh
cat files.info >> ./z_check.sh
echo "" >> ./z_check.sh

chmod 0755 ./z_check.sh
./z_check.sh
rm ./z_check.sh
