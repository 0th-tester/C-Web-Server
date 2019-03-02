# MIME 타입(MIME types)

_MIME(Multipurpose Internet Mail Extensions)_  은 원래 이메일을 위해 만들어졌다.

과거에는 모든 이메일은 ASCII 텍스트로만 작성할 수 있었다. 이미지를 첨부하기 원한다면 ASCII-art로 그려야만 했다...

그래서 다른 종류의 데이터도 이메일로 전달하기 위해 MIME이 개발되었다. 
그것은 이메일이 여러 부분을 포함하도록 허용했고, 데이터 유형을 식별하는 방법이 각 파트 안에 포함되어 있었다.

MIME은 여전히 최근까지 이메일에 사용된다.

그러나 웹의 설계자들은 같은 문제들 중 많은 것들이 웹과 함께 MIME을 사용함으로써 해결될 수 있다는 것을 알게됬다.

문제 중 하나는 서버로부터 도착하는 데이터의 타입을 식별하는 것이다.

## What type is my data?

바이트는 그냥 숫자들의 집합니다. 그래서 웹 브라우저가 서버로부터 바이트 스트림을 받을 때,  
이것이 유니 코드 HTML 데이터, 바이너리 GIF 데이터 또는 JPEG 사진인지 어떻게 알 수 있을까? 
그것들은 오직 숫자들의 연속된 값일 뿐이다.

서버는 브라우저에 이 데이터의 타입이 무엇이라고 알려주어야 한다. 그리고 이것에 의해 MIME 유형을 지정합니다.

흔히 사용되는 MIME 타입이다.:

* JPEG: `image/jpeg`
* GIF: `image/gif`
* PNG: `image/png`
* JavaScript: `application/javascript`
* JSON: `application/json`
* CSS: `text/css`
* HTML: `text/html`
* Plain TXT: `text/plain`
* Non-descript data: `application/octet-stream`

> _옥텟(Octet)_ 은 8비트이며, _바이트(byte)_ 로 흔히 알려져 있다.
> (역사적으로 한 바이트의 비트 수는 다양했지만, 오늘날 바이트는 8비트이다.)

There are [a lot of MIME
types](https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Complete_list_of_MIME_types).

## 데이터 유형을 결정하는 방법은 무엇인가?

### 프로그래밍 방식 종단점(Programmatic endpoints)

프로그래밍 방식 종단점(endpoint)(디스크에서 데이터를 읽는 대신 데이터를 생성하는 부분)을 사용하는 경우 보내려는 데이터의 유형을 지정하기만 하면 된다.

예를 들어, 반환 데이터가 다음과 같다면:

```json
{
    "animal_type": "goat",
    "count": 37
}
```

_Content-Type_ 을 `applicaion/json`을 사용 해야 한다.

### 파일 전달

하지만 파일로부터 데이터를 읽거나 보내는 경우는 어떨까?

클라이언트가 `http://example.com/foo.png`로 요청을 보낸다면, 우리는 `image/png`으로 응답 할 필요가 있다.
위와 같이 동작하기 위해서, 일반적인 방법은 단순히 파일 확장자 `.png`와 MIME 타입 `image/png` 간에 매핑하는 것이다.

1. 파일 확장자를 분리해라.

   Example:
   
   `frotz.jpg` 파일의 확장자는 `.jpg` 이다.

   `foo.bar.txt` 파일의 확장자는 `.txt` 이다.

2. MIME 타입과 확장자간 매핑하기.

   Example: `.txt`는 `text/plain`와 매핑한다.

3. 매핑할 확장자를 찾지 못하면, `application/octet-stream`으로 지정한다.

확장자 비교는 대소문자를 비교하지 않는다. 

Example:

* `.jpg` or `.jpeg`: `image/jpeg`
* `.gif`: `image/gif`
* `.png`: `image/png`
* `.js`: `application/javascript`
* `.json`: `application/json`
* `.css`: `text/css`
* `.htm` or `.html`: `text/html`
* `.txt`: `text/plain`
* `.ico`: `image/x-icon`

## MIME 타입은 어떻게 반환될까?

HTTP 헤더 안에 `Content-Type` 필드로 돌아가보자.:

```
Content-Type: text/html
```

어떤 라이브러리를 사용하든 `Content-Type`을 설정할 방법이 있어야 한다.

웹 서버는 `Content-Type`과 생성된 시간이 포함된 자신만의 HTTP 헤더를 생성한다. 

## MIME 타입에 따라 브라우저는 무엇을 할까?

브라우저는 MIME 타입을 데이터를 구별하기 위해 사용한다.
`text/html` 이라면, 화면에 출력할 것이다.
아마 `image/*`, `text/plain` 이여도 마찬가지이다.

브라우저는 많은 종류의 데이터를 자연스럽게 표현 할 수 있다. 하지만, 브라우저가 모르는 MIME 타입이면 어떠할까? 약 1.2 억 개의 MIME 유형이 있다. 

경우에 따라 두 가지 옵션이 있다.:

1. 파일을 아는 경우 외부 뷰어를 생성하여 파일을 표시 할 수 있습니다.
2. 단순히 파일을 다운로드하여 나중에 사용자가 처리하도록 한다.
