package main

import (
	"fmt"
	"net/http"
	"runtime"
)

func main() {
	// Установить максимальное количество потоков для использования
	numCPU := runtime.NumCPU()
	runtime.GOMAXPROCS(numCPU)
	fmt.Printf("Using %d CPU cores\n", numCPU)

	// Обработчик для корневого маршрута
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusOK)
		_, _ = w.Write([]byte("Hello!"))
	})

	// Создание сервера
	server := &http.Server{
		Addr:    "localhost:8000",
		Handler: nil, // Используем глобальный HTTP-маршрутизатор
	}

	// Запуск сервера
	fmt.Println("Server is running on http://localhost:8000")
	if err := server.ListenAndServe(); err != nil {
		fmt.Printf("Server failed: %s\n", err)
	}
}

