#!/bin/bash
#
# RUN - run K19 tests
#
# usage:
#	RUN [ name.obj ... ] 
#
#	name.obj:	run only the listed object file(s)

program="../../cmake-build-debug/p2/pdp11"
list=""

#
# process command-line arguments
#

if [ $# -eq 0 ]
then

	#
	# none - test all the object files
	#
	list="$list *.obj"

else

	#
	# test only the specified object files
	#
	while [ $# -gt 0 ]
	do
		list="$list $1"
		shift
	done

fi

#
# start the testing
#
for f in $list
do

	printf "Testing %s ..." $f

	#
	# run the simulator on this object file
	#
	bn="`basename $f .obj`"
	rm -f $bn.mine $bn.diffs
	./$program $f | grep -v Simulat > $bn.mine

	#
	# see if the results match exactly
	#
	grep -v Simulat $bn.out | cmp -s $bn.mine
	if [ $? -eq 0 ]
	then
		printf " OK\n"
		rm -f $bn.mine
	else
		#
		# no - generate a difference listing
		#
		diff $bn.out $bn.mine > $bn.diffs
		printf " your output in %s, diffs in %s\n" $bn.mine $bn.diffs
	fi

done
