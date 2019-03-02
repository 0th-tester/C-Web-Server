# A Simple Web Server in C

이 프로젝트에서는 C 언어로 웹 서버 구현을 완료합니다.

구현할 것들:

  * HTTP request parser
  * HTTP response builder
  * LRU cache
  * Doubly linked list 
  * hashtable

* 코드는 기존의 코드와 연동될 것이다. 
기존의 소스를 이해하는 것은 이번 과제의 예상되는 부분이다.

기존의 것들:

* `net.h`, `net.c` 에는 low-level의 네트워킹 코드를 포함한다.
* `mime.h`, `mime.c`에는 파일의 MIME 타입을 결정하는 것을 위한 기능을 포함한다.
* `file.h`, `file.c`에는  파일 데이터 읽기와 파일 데이터 할당 해제를 위한 `file_load ()`와`file_free ()`함수를 이용 하기를 원한다. 이 작업을 수동으로 수행 할 수도 있다)
* `hashtable.h`, `hashtable.c`에는 해시테이블이 구현되어 있다. (이것은 Hashtables 스프린트에서 빌드 한 것보다 조금 더 복잡하다.)
* `llist.h`, `llist.c`에는 이중 연결 리스트가 구현되어 있다. 
* `cache.h`, `cache.c`은 3, 4일째 날에 LRU 캐시를 구현해야하는 곳이다.

## 웹 서버란?

웹 서버는 HTTP 요청을 허용하는 소프트웨어입니다 (예 : GET
HTML 페이지에 대한 요청), 응답 (예 : HTML 페이지)을 반환한다. 다른 일반적인 용도로는 RESTful API 엔드포인트, 웹 페이지 내의 이미지 및 서버에 데이터 업로드를위한 POST 요청 (예 : 양식 제출 또는 파일 업로드)에서 데이터를 가져 오는 GET 요청이 있다.

## 읽을거리

* [Networking Background](guides/net.md)
* [Doubly-Linked Lists](guides/dllist.md)
* [LRU Caches](guides/lrucache.md)
* [MIME types](guides/mime.md)

## 과제

우리는 여러 파일과 어떤 데이터를 특정 엔드포인트에 반환하는 간단한 웹 서버를 구현할 것이다.

* `http://localhost:3490/d20`은  1과 20 사이의 무작위 수를 `text/plain`로 반환해야한다.
* 다른 URL은 `serverroot` 디렉토리와 파일에 매핑 되어야 한다. 예를 들어:

  ```
  http://localhost:3490/index.html
  ```

  should serve the file

  ```
  ./serverroot/index.html
  ```


`server.c`와`cache.c`에있는 스켈레톤 소스 코드에서 구현해야 할 부분을 검사해라.

**중요** _코드를 인벤토리 화하여 시간과 장소를 확인하십시오. 메모를 적어 두십시오. 개요를 작성하십시오. 어떤 함수가 다른 함수를 호출하는지 주목하십시오. 이 작업을 수행하는 데 시간을 소비하면 전반적으로 소요되는 시간이 줄어 든다._

_기존의 코드는 문제에 접근하는 데 큰 힌트이다._

이미 작성된 부분은 적당히 잘 주석 처리 된 코드를 연구하여 어떻게 작동하는지 확인하자.

`Makefile`이 제공된다. 서버를 빌드 하기 위해 명령어로 `make` 입력한다.

서버 실행을 위해 `./server`를 입력한다.

### Main Goals

_Read through all the main and stretch goals before writing any code to get an overall view, then come back to goal #1 and dig in._

#### Days 1 and 2

1. `send_response()` 구현하기.

   이 함수는 클라이언트가 기대하는 적절한 형식으로 HTTP 응답을 구성하는 모든 부분을 형식화한다.  다시말해, 주어진 파라미터에 따라 완벽한 HTTP 응답을 구현해야 한다. 그것은 `response` 변수에 문자열에 대한 응답을 작성 해야한다.
   
   헤더와 본문의 총합 길이는 response_length에 저장되어야 한다. `send ()`호출은 얼마나 많은 바이트를 전송할지 알 수 있다.


   HTTP 응답의 예제는 위의 [HTTP](#http) 섹션을 참조하고 직접 작성하여 사용해라.

   Hint: `sprintf()`는 HTTP response 생성을, `strlen()`은 content-length 계산을 한다.
   `sprintf()`는 결과 문자열에있는 총 바이트 수를 반환하는데 도움이 될 수 있있다. 
   응답에 Date 필드의 현재 시각을 얻기 위해, `time()`, `localtime()`을 참고하길 원한다. 둘 다 `time.h` 헤더 파일에 포함되어 있다.

   > HTTP `Content-Length` 헤더는 헤더를 제외한 본문 길이만 포함 해야한다.
   > 하지만, `send()`에 사용되는 `response_length` 변수는 헤더와 본문의 전체 길이이다.

   `main` 함수의 어딘가에서 `resp_404` 함수를 호출하고 클라이언트가 404 응답을 수신 했는지 확인하여 `send_response` 작업을 했는지 여부를 테스트 할 수 있다.

2. `server.c` 파일 내 `handle_http_request()` 검사하기.

   HTTP 요청 헤더의 첫 번째 행을 구문 분석하여 이것이 `GET` 또는`POST` 요청인지 확인하고 경로가 무엇인지 확인하고자 할 것이다.
   어떤 핸들러 함수를 호출할 지 결정하기 위해 이러한 정보를 사용할 것이다.

   `handle_http_request ()`의 변수`request`는 `recv ()`호출이 돌아 오면 전체 HTTP 요청을 유지한다.

   HTTP 헤더의 첫째 줄로 세개 컴포넌트를 읽는다. Hint: `sscant()`.

   그 직후에 요청 유형 (`GET`,`POST`)과 경로 (`/ d20` 또는 다른 파일 경로)에 따라 적절한 핸들러를 호출하십시오. `/ d20`을 확인한 후 나중에 임의의 파일을 추가 할 수 있습니다.

   Hint: `strcmp()`으로 요청 메소드나 경로가 일치되는 지 확인. `strcmp()`는 문자열이 _같다면_ `0` 을 반환한다.

   > Note: C 언어에선 문자열에`switch ()`를 사용할 수 없다. 문자열 내용 대신 문자열 포인터 값을 비교할 것이기 때문이다.
   > 작업을 완료하려면 `strcmp ()`와 함께`if`-`else 블록을 사용해야 한다.


   적절한 핸들러를 찾을 수 없다면 대신`resp_404 ()`를 호출하여 "404 Not Found" 응답을 보내라.

3. `get_d20()` 핸들러를 구현해라. `send_response()`를 호출할 것이다.

   `get_d20 ()`이`send_response ()`에 전달 해야 하는 것에 대한 할당의 시작 부분을 확인해라.
   
   `send_response ()`호출이 어떻게 보이는지에 대한 힌트가 필요하다면, 바로 위의`resp_404 ()`에서 그것의 사용법을 확인해라.
   
   파일 내용을 되돌려 보내는 다른 응답과는 달리,`d20` 엔드 포인트는 단순히 난수를 계산하여 돌려 보낼 것이다. 파일에서 번호를 읽지 않는다.

   > 모든 함수에 널리 전달되는`fd` 변수는 _file descriptor_를 가지고 있습니다. 열린 커뮤니케이션 경로를 나타내는 데 사용되는 숫자이다. 
   > 일반적으로 디스크의 일반 파일을 가리 키지만, 이 경우에는 _socket_ 네트워크 연결을 가리킨다. 
   > `fd`를 생성하고 사용하는 모든 코드는 이미 작성되었지만 여전히 사용 된 지점으로 전달해야 한다.

4. 임의 파일 제공을 구현하기.

   다른 URL은 `serverroot` 디렉토리와 그 내부에 있는 파일에 매핑 되어야 한다. 예시 :

   `http://localhost:3490/index.html`은 `./serverroot/index.html` 파일 제공.

   `http://localhost:3490/foo/bar/baz.html`은 
   `./serverroot/foo/bar/baz.html` 파일 제공.

   이것을 실현하기 위해 `file.c`의 기능을 사용할 수 있다.

   
   또한 파일에 있는 데이터에 따라 `Content-Type` 헤더를 설정 해야 한다. `mime.c`는 이것에 유용한 기능을 가지고 있다.

#### Days 3 and 4

LRU 캐시 구현해라. 파일은 RAM에 파일을 캐시하는 데 사용되므로 OS를 통해 파일을 로드 할 필요가 없다.

파일이 요청 받을 때 캐시는 파일이 캐시에 있는 지 확인 해야 한다. 있다면, 캐시에서 파일을 제공한다. 없다면, 파일을 디스크에서 로드하고 제공하고 캐시에 저장한다.

캐시는 엔트리의 최대 수를 가진다. 엔트리기 최대 수를 넘게 되면, LRU에 맞게 엔트리는 삭제된다.

LRU 캐시는 [이중 연결 리스트](https://en.wikipedia.org/wiki/Doubly_linked_list) 와 [해시 테이블]((https://en.wikipedia.org/wiki/Hash_table))로 구성되어 있다.

해시 테이블 코드는 이미 `hashtable.c`에 구현되어 있다.

1. `cache.c` 내 `cache_put()` 함수 구현하기.

알고리즘:

* 파라미터에 따라 새 캐시 엔트리를 할당한다.

   * 이중 연결 리스트의 맨 앞에 엔트리를 넣는다.
   * 그 엔트리의 `path`를 인덱스로 하는 해시 테이블에 저장한다.
   * 캐시의 현재 사이즈를 증가하다.
   * 캐시 사이즈가 최대 치보다 클 경우:
     * 리스트의 맨 끝 엔트리를 삭제한다.
     * 엔트리의 `path`와 `hashtable_delete` 함수를 이용해 해시테이블에 같은 엔트리를 제거한다.
     * 캐시 엔트리를 할당 해제한다.
     * 카운터 크기와 캐시 내에 엔트리 수가 같은지 확인해라

2.  `cache.c` 내 `cache_get()` 함수 구현하기.

  알고리즘:
   
   * 해시 테이블에서 'path'로 캐시 항목 포인터를 찾으려고 시도합니다.
   * 없다면 , `NULL` 값을 반환.
   * 이중 연결 리스트의 맨 앞에 엔트리를 이동
   * 캐시 엔트리 포인터 반환.

3. `server.c` 캐시 기능 추가하기

  파일이 요청될 때, 그 경로에 파일이 캐시 상에 있는 지 처음 확인한다.

  있다면, 반환해준다.

없다면:
  * 디스트에서 파일을 로드한다. (`file.c` 확인)
  * 캐시 안에 저장한다.
  * 제공한다.

캐시 구현한 것이 정확한지 확인하기 위해 유닛 테스트를 해놓았다. 구현한 것을 바탕으로 유닛 테스트를 하기 위해 `src` 디렉토리에서 `make tests`를 수행한다.

### Stretch Goals

#### 1. 파일 게시

1. HTTP 요청 본문의 시작 부분을 찾기 위해 `find_start_of_body()` 를 구현한다.

2. `post_save()` 핸들러를 구현해라. 본체를 본체에 전달하도록 기본 루프를 수정해라. 
이 핸들러에서 파일을 디스크에 쓰도록 한다. Hint :`open ()`,`write ()`,`close ()`. `fopen ()`,`fwrite ()`,`fclose ()` 변종도 사용할 수 있지만 앞의 세 함수는 이 경우 약간 더 사용하기 쉽다.


`post_save()`의 응답은 `application/json` 유형이어야 하며 `{"status": "ok"}` 이어야 한다.


#### 2. `index.html` 자동 전달


우리는 사용자가 `http://localhost:3490/index.html`을 입력한 경우, `./serverroot/index.html` 에 파일을 리턴 해야 한다는 것을 알고 있다.

사용자가 `http://localhost:3490/`(엔드포인트 인 `/`, 디스크 상에 `./serverroot/`)에 접근하면 파일이 발견되지 않으면 `index.html`길 끝까지 가서 다시 시도해라.



 `http://localhost:3490/` 로 처음 시도 시:

```
./serverroot/
```

못찾으면 다음과 같이:

```
./serverroot/index.html
```

성공.

#### 3. 캐시 엔트리 만료

영원히 캐쉬하는 것은 의미가 없습니다. 파일이 디스크에서 변경되면 어떻게 될까요?

캐시 엔트리에`created_at` 타임 스탬프를 추가해라.


항목이 캐시에 있으면 1 분 이상 경과했는지 확인해라. 그럴 경우 캐시에서 삭제 한 다음 디스크에서 새 것으로 로드 해라.


캐시에서 너무 오래된 항목을 실제로 제거하는 작업을 수행하는 캐시 코드에`cache_delete` 함수를 추가해야 한다.


#### 4. 동시성

_난이도: Pretty Dang Tough_


pthreads 라이브러리를 연구하십시오.

새로운 연결이 생기면 그것을 처리 할 스레드를 시작하십시오.

쓰래드가 서로의 캐시를 손상시키지 않도록 쓰레드가 쓰레드를 액세스 할 때 캐시를 잠그십시오.

또한 죽은 스레드를 처리 할 수 있는 스레드 정리 핸들러가 있어야 한다.

