package main

import (
	"bufio"
	"fmt"
	"os"
)

func isAlphanumeric(r rune) bool {
	return (r >= 'A' && r <= 'Z') || (r >= 'a' && r <= 'z') || (r >= '0' && r <= '9')
}

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Usage: go run main.go <filename>")
		return
	}

	filename := os.Args[1]

	file, err := os.Open(filename)
	if err != nil {
		fmt.Printf("Error opening file: %v\n", err)
		return
	}
	defer file.Close()

	scanner := bufio.NewReader(file)
	var buffer []rune
	const minLength = 6

	for {
		r, _, err := scanner.ReadRune()
		if err != nil {
			break
		}

		if isAlphanumeric(r) {
			buffer = append(buffer, r)
		} else {
			if len(buffer) >= minLength {
				fmt.Println(string(buffer))
			}
			buffer = buffer[:0]
		}
	}

	if len(buffer) >= minLength {
		fmt.Println(string(buffer))
	}
}
