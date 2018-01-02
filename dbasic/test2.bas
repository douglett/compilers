; set times, then run
function printass
	print "here2"
	let i 0
	while (< times i)
		print "ass" i
		let i (+ i 1)
	end
	print "here3"
end

; go
print "here1"
let times 10
call printass