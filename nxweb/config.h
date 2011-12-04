#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#define NXWEB_LISTEN_PORT 8088
#define N_NET_THREADS 4
#define N_WORKER_THREADS 4
#define READ_REQUEST_TIMEOUT 10.
#define WRITE_RESPONSE_TIMEOUT 10.
#define KEEP_ALIVE_TIMEOUT 30.
#define REQUEST_CONTENT_SIZE_LIMIT 524288
#define DEFAULT_CHUNK_SIZE 8192
#define OUTPUT_CHUNK_SIZE 16384
#define OUTPUT_CHUNK_MIN_ROOM 256
#define NXWEB_JOBS_QUEUE_SIZE 8192
#define NXWEB_ACCEPT_QUEUE_SIZE 4096
#define NXWEB_PID_FILE "nxweb.pid"

#endif // CONFIG_H_INCLUDED
