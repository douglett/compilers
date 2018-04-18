# reverse a list in place
let list "apple;banana;pear;pineapple;grape"
print "before:" list

let i "0"
arrlen list
sub pop "1"
let j pop
do
	# swap in place
	arrat list i
	let t pop
	arrat list j
	arrset list i pop
	arrset list j t
	# increment and check if we should loop
	add i "1"
	sub j "1"
	lt i j
	if pop then
		continue
	end
end

print "after:" list