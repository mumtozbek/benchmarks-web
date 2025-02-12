const std = @import("std");
const net = std.net;
const Thread = std.Thread;

const RESPONSE = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nHello!";

fn handle_client(client: net.Stream) void {
    defer client.close();
    var buf: [1024]u8 = undefined;
    _ = client.read(&buf) catch return; // Читаем, но не обрабатываем запрос
    _ = client.writeAll(RESPONSE) catch return;
}

pub fn main() !void {
    var gpa = std.heap.page_allocator;
    var server = try net.StreamServer.init(.{});
    defer server.deinit();

    try server.listen(net.Address.parseIp("0.0.0.0", 8000) catch unreachable);

    while (true) {
        var client = server.accept() catch continue;
        _ = Thread.spawn(.{}, handle_client, .{client}) catch continue;
    }
}
