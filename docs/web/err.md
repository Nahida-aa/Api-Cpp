我的 8080是被占用的，但是为什么没有报 端口被占用，我发现问题可能是我没有权限使用 127.0.0.1:8080 。 因为 ，我使用 127.0.0.1:8080 报的是 10013 (WSAEACCES: Permission denied ”)，使用 localhost:8080 报的是 10048 (WSAEADDRINUSE) ， 但是我使用 127.0.0.1:8081 却是没有问题，这就有点奇怪了

- 10013 (WSAEACCES: 访问被拒绝, Permission denied), 这表明另一个应用程序可能正在使用这个特定的IP：端口组合，并具有独占权
- 10048 (WSAEADDRINUSE: 地址已被使用, Address already in use)