# print 1 to n, but only multiples of 3 or 5
#
print "input a number"
input n
let i "1"
do
	# check if we should print
	# check 5 multiplier
	push i
	mod pop "3"
	eq pop "0"
	# check 5 multiplier
	push i
	mod pop "5"
	eq pop "0"
	# if either print
	add pop pop
	gt pop "0"
	if pop then
		print i
	end

	# next i
	lt i n  # i < n
	if pop then
		add i "1"
		continue
	end
end