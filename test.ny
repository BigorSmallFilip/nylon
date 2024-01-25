function find_longest_word(str)
	words = []
	current_word = ''

	# Get all the words
	for c in str
		if c != ' '
			current_word += c
		else
			words.push(current_word)
			current_word = ''

	# Find the longest of them
	longest_word = ''
	for word in words
		if word.length > longest_word.length
			longest_word = word

	return longest_word



function secundo()
	if 5 == 5
	print('e')