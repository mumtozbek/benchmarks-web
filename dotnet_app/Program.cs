var builder = WebApplication.CreateBuilder(args);
var app = builder.Build();

app.MapGet("/", () => "Hello!");

app.Run("http://0.0.0.0:8000");

