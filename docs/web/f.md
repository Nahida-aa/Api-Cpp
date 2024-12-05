# web framework

- web server: 负责处理底层的网络通信，接受客户端连接，接收请求并发送响应。这是一个软件，负责监听网络请求并将请求转发给 Web 框架处理，同时将框架生成的响应返回给客户端。Web 服务器通常还处理一些低级别的网络通信细节。常见的 Web 服务器包括 Uvicorn 、Gunicorn、Nginx、Apache 等。
- web framework: 提供路由、请求处理、响应生成等高级功能，简化 Web API 的开发。是一个库或工具集，帮助开发者更方便地编写 Web 应用程序。Web 框架提供了路由、请求处理、响应生成、模板渲染等功能。常见的 Web 框架包括 FastAPI、Django、Flask 等
- web api application: 用户定义的具体业务逻辑和处理函数，响应特定的 API 请求。包括处理请求、访问数据库、执行计算、返回响应等。这个部分是应用程序的核心，定义了 API 的具体行为和功能。
