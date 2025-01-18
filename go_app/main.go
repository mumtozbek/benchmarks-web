package main

import (
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
    w.Write([]byte("Hello!"))
}

func main() {
	http.HandleFunc("/", handler)
	http.ListenAndServe(":8000", nil)
}
