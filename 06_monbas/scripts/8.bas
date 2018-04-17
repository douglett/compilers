# find prime numbers
let max "100"  # total to search to
let i "1"
do
	# set divisor
	let j "2"
	do
		push i
		mod pop j
		ifn pop then
			break  # division found
		end
		# move to next divisor
		add j "1"
		lt j i
		if pop then
			continue
		end
		# prime found!
		print "prime:" i
	end

	# loop
	add i "1"
	lt i max
	if pop then
		continue
	end
end