/**
 * webserver.c -- A webserver written in C
 * 
 * Test with curl (if you don't have it, install it):
 * 
 *    curl -D - http://localhost:3490/
 *    curl -D - http://localhost:3490/d20
 *    curl -D - http://localhost:3490/date
 * 
 * You can also test the above URLs in your browser! They should work!
 * 
 * Posting Data:
 * 
 *    curl -D - -X POST -H 'Content-Type: text/plain' -d 'Hello, sample data!' http://localhost:3490/save
 * 
 * (Posting data is harder to test from a browser.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/file.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include "net.h"
#include "file.h"
#include "mime.h"
#include "cache.h"

#define PORT "3490"  // the port users will be connecting to

#define SERVER_FILES "./serverfiles"
#define SERVER_ROOT "./serverroot"

pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER; 
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct args {
    int fd;
    struct cache* cache;
};

/**
 * Send an HTTP response
 *
 * header:       "HTTP/1.1 404 NOT FOUND" or "HTTP/1.1 200 OK", etc.
 * content_type: "text/plain", etc.
 * body:         the data to send.
 * 
 * Return the value from the send() function.
 */
int send_response(int fd, char *header, char *content_type, void *body, int content_length)
{
    const int max_response_size = 262144;
    char response[max_response_size];

    // Build HTTP response and store it in response

    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    int response_length = sprintf(
        response,
        "%s\nDate: %sConnection: close\nContent-Length: %d\nContent-Type: %s\n\n",
        header,
        asctime(timeinfo),
        content_length,
        content_type
    );

    memcpy(response + response_length, body, content_length);
    
    response_length += content_length;
    // printf(response);
    // Send it all!
    int rv = send(fd, response, response_length , 0);

    if (rv < 0) {
        perror("send");
    }

    return rv;
}


/**
 * Send a /d20 endpoint response
 */
void get_d20(int fd)
{
    // Generate a random number between 1 and 20 inclusive
    srand(time(NULL));
    int random = rand() % 20 + 1;
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char response[8];
    sprintf(response, "%d", random);
    // Use send_response() to send it back as text/plain data
    char *mime_type = "text/plain";
    // mime_type = mime_type_get("txt");

    send_response(fd, "HTTP/1.1 200 OK", mime_type, response, strlen(response));
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
}

/**
 * Send a 404 response
 */
void resp_404(int fd)
{
    char filepath[4096];
    struct file_data *filedata; 
    char *mime_type;

    // Fetch the 404.html file
    snprintf(filepath, sizeof filepath, "%s/404.html", SERVER_FILES);
    filedata = file_load(filepath);

    if (filedata == NULL) {
        // TODO: make this non-fatal
        fprintf(stderr, "cannot find system 404 file\n");
        exit(3);
    }

    mime_type = mime_type_get(filepath);

    send_response(fd, "HTTP/1.1 404 NOT FOUND", mime_type, filedata->data, filedata->size);

    file_free(filedata);
}

/**
 * Read and return a file from disk or cache
 */
void get_file(int fd, struct cache *cache, char *request_path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    char filepath[4096];
    struct file_data * filedata;
    char *mime_type;

    filedata = ( struct file_data * ) malloc(sizeof(struct file_data));

    sprintf(filepath, "%s/%s", SERVER_ROOT, request_path);
    
    struct cache_entry * cur_entry = cache_get(cache, filepath);

    time_t cur_time;

    if ( cur_entry == NULL ) {
    
        filedata = file_load(filepath);  
        mime_type = mime_type_get(request_path);  
        cache_put(cache, filepath, mime_type, filedata->data, filedata->size);

    } else {

        filedata->data = cur_entry->content;
        filedata->size = cur_entry->content_length;
        mime_type = cur_entry->content_type;

        cur_time = time(NULL);
        // printf("cached\n"); 

        if ( difftime( cur_time, cur_entry->create_at ) / 60 >= 1 ) {
            // printf("timeout new cached\n"); 
            cache_delete(cache, cur_entry);

            filedata = file_load(filepath);  
            mime_type = mime_type_get(request_path);  
            cache_put(cache, filepath, mime_type, filedata->data, filedata->size);
        
        }
    }
    // printf("%s\n", filedata->data);
    send_response(fd, "HTTP/1.1 200 OK", mime_type, filedata->data, filedata->size);
    
    if ( cur_entry == NULL ) {
        file_free(filedata);
        filedata = NULL;
    }
}

/**
 * Search for the end of the HTTP header
 * 
 * "Newlines" in HTTP can be \r\n (carriage return followed by newline) or \n
 * (newline) or \r (carriage return).
 */
char *find_start_of_body(char *header)
{
    ///////////////////
    // IMPLEMENT ME! // (Stretch)
    ///////////////////
    char *find_body;
    
    find_body = strstr(header, "\n\n");
    if( find_body != NULL ) return find_body;

    find_body = strstr(header, "\r");
    if( find_body != NULL ) return find_body;

    find_body = strstr(header, "\n");
    if( find_body != NULL ) return find_body;
    // printf("%s", find_body);
    return find_body;
}

/**
 *  post_save
 */
void post_save(int fd, char * body)
{
    // printf("%s", body);
    char response[128];
    char *status; 
    char *mime_type = mime_type_get("json");
    int file_fd = open("data.txt", O_CREAT | O_RDWR );

    if ( file_fd > 0 ) {
    
        flock(file_fd, LOCK_EX);
        // printf("%s %d", body, strlen(body));
        write(file_fd, body, strlen(body));
        close(file_fd);

        sprintf(response, "{\"%s\":\"%s\"}", "status", "ok");
        
        status = "HTTP/1.1 200 OK";

    } else {
        
        sprintf(response, "%s", "check args");
        
        status = "HTTP/1.1 400 Bad Request";
    
    }
    send_response(fd, status, mime_type, response, strlen(response));
}
/**
 * Handle HTTP request and send response
 */
void handle_http_request(int fd, struct cache *cache)
{
    const int request_buffer_size = 65536; // 64K
    char request[request_buffer_size];

    // Read request
    int bytes_recvd = recv(fd, request, request_buffer_size - 1, 0);

    if (bytes_recvd < 0) {
        perror("recv");
        return;
    }


    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Read the three components of the first request line

    // If GET, handle the get endpoints

    //    Check if it's /d20 and handle that special case
    //    Otherwise serve the requested file by calling get_file()


    // (Stretch) If POST, handle the post request
    char method[8];
    char endpoint[512];
    char protocol[20];
    // printf("%s\n", request);
    sscanf(request, "%s %s %s", method, endpoint, protocol);
    printf("%s\n", endpoint);
    
    if ( strcmp(method, "GET") == 0 ) {
         if ( strcmp(endpoint, "/d20") == 0 ) {
             get_d20(fd);
         } else if ( strcmp(endpoint, "/index.html") == 0 ) {
             get_file(fd, cache, endpoint);
         } else if ( strcmp(endpoint, "/cat.jpg") == 0 ) {
             get_file(fd, cache, endpoint);
         } else {
             resp_404(fd);
         }
    } else if ( strcmp(method, "POST") == 0 ) {
        // printf("%s", request);
        char *body = find_start_of_body(request);
        // printf("%s", body);
        post_save(fd, body);
        // free(body);
        body = NULL;
        // resp_404(fd);
    }
}

void thread_cleanup(void * input)
{
    // printf("thread is clean up\n");
    struct args * p_args = (struct args*)input;
    close(p_args->fd);
}

void * connection_handle(void * input)
{

    pthread_detach(pthread_self()); 

    struct args * p_args = (struct args*)input;

    pthread_cleanup_push(thread_cleanup, (void*)p_args); 
    // printf("thread id : %lu \n", pthread_self());
    if(pthread_mutex_trylock(&mutex) == 0) {
        
        // printf("test_mutex_try_lock \n");
        handle_http_request(p_args->fd, p_args->cache);
        // handle_http_request(p_args->fd[p_args->id], p_args->cache);
        pthread_mutex_unlock(&mutex);
    
    } else {
        
        printf("failed to get lock\n");

    }

    pthread_exit(0);

    pthread_cleanup_pop(0);

}


void atexit_clean(void *data)
{
    static struct args *resource;

    if (data) {
        resource = data;
        atexit(clean);
    } else {
        pthread_mutex_destroy(&mutex);
        cache_free(resource->cache);
    }
}

static void clean(void)
{
    // printf("clean\n");
    atexit_clean(NULL);
}


void handle_sigint(int sig)
{
    printf("Caught signal %d\n", sig); 
    // cache_free(cache);
    exit(EXIT_SUCCESS);
}
/**
 * Main
 */
int main(void)
{
    int t_id;
    int p_id = 0;
    pthread_t p_thread;
    struct args p_args;

    int newfd;  // listen on sock_fd, new connection on newfd
    struct sockaddr_storage their_addr; // connector's address information
    char s[INET6_ADDRSTRLEN];

    struct cache *cache = cache_create(10, 0);

    // Get a listening socket
    int listenfd = get_listener_socket(PORT);

    if (listenfd < 0) {
        fprintf(stderr, "webserver: fatal error getting listening socket\n");
        exit(1);
    }

    printf("webserver: waiting for connections on port %s...\n", PORT);
    
    p_args.fd = -1;
    p_args.cache = cache;

    // This is the main loop that accepts incoming connections and
    // forks a handler process to take care of it. The main parent
    // process then goes back to waiting for new connections.
    // pthread_t p_thread;
     
     atexit_clean(&p_args);
     signal(SIGINT, handle_sigint);

    while(1) { 
        socklen_t sin_size = sizeof their_addr;

        // Parent process will block on the accept() call until someone
        // makes a new connection:
         p_args.fd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        if (p_args.fd == -1) {
            perror("accept");
            continue;
        }
        
        // // Print out a message that we got the connection
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        
        // newfd is a new socket descriptor for the new connection.
        // listenfd is still listening for new connections.
        // resp_404(newfd);
        // handle_http_request(newfd, cache);
        // printf("%d",  p_args[p_id].fd );
        // printf("p_args[p_id].fd , %d \n", p_args.fd[p_id]);        
        t_id = pthread_create(&p_thread, NULL, connection_handle, (void *)&p_args);
        // printf("\np_id %d p_thread %lu \n" , p_id, p_thread);
        if ( t_id < 0 ) {
            perror("thread create error:");
            exit(EXIT_FAILURE);
        }

    }
    // Unreachable code
    pthread_mutex_destroy(&mutex);
    cache_free(cache);

    return EXIT_SUCCESS;
}

