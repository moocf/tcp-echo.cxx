A multi-echo supporting TCP echo server and client.

<br>


```bash
## SERVER
$ clang main.cxx
$ ./a.out server

# Server listening on port 5000
NOTE: Now start the client in another terminal
# Client connected: 127.0.0.1:49847
# Received message: Hello, World! (13 bytes)
# Sent echo: Hello, World! (13 bytes)
NOTE: You can start multiple clients, each in a separate terminal
# Client connected: 127.0.0.1:49848
# Received message: Hello, World! (13 bytes)
# Sent echo: Hello, World! (13 bytes)
```

<br>


```bash
## CLIENT 1
$ ./a.out client

# Sent message: Hello, World! (13 bytes)
# Received echo: Hello, World! (13 bytes)
```

<br>


```bash
## CLIENT 2
$ ./a.out client

# Sent message: Hello, World! (13 bytes)
# Received echo: Hello, World! (13 bytes)
```

<br>
<br>


[![](https://raw.githubusercontent.com/qb40/designs/gh-pages/0/image/11.png)](https://wolfram77.github.io)<br>
[![ORG](https://img.shields.io/badge/org-moocf-green?logo=Org)](https://moocf.github.io)
![](https://ga-beacon.deno.dev/G-G1E8HNDZYY:v51jklKGTLmC3LAZ4rJbIQ/github.com/moocf/tcp-echo.cxx)
