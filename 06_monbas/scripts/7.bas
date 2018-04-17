# multiplication table for numbers to 12
let i "1"
do
	# print lint for i
	let j "12"
	do
		gt j "0"
		if pop then
			push i
			mul pop j
			sub j "1"
			continue
		end
		print pop pop pop pop   pop pop pop pop   pop pop pop pop
	end
	# next i
	add i "1"
	gt i "12"
	ifn pop then
		continue
	end
end