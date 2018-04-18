# lists
let values "1;2;3;4;3;2;1"
arrlen values
let len pop
print "values:" values
print "length:" len

let i "0"
arrat values i
let max pop
do
	arrat values i
	# print "val:" peek
	gt peek max
	if pop then
		let max peek
	end
	pop
	# next
	add i "1"
	lt i len
	if pop then
		continue
	end
end

# show result
print "highest number:" max