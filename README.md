# OmokServer
### 프로젝트 소개
IOCP로 구현한 네트워크 오목 게임 서버입니다.

### 개발 엔진 및 언어
C++ 서버 & Unity

### 구현 기능
<p>1. C++ 서버 - C# 클라이언트 간 네트워크 연결</p> 
<p>2. IOCP 비동기 데이터 송수신</p>
<p>3. 로비 접속 유저 간 채팅</p>
   
![image](https://github.com/user-attachments/assets/95ea45e3-4fbc-4bd7-a04c-08936e6ed936)

<p>유저 접속/종료 시 메시지, 채팅 메시지, 접속 유저 목록, 입장 가능한 방 목록 확인 가능</p>

<p>4. 유저 간 오목 게임</p>
 
![image](https://github.com/user-attachments/assets/85f8cbeb-a0b9-4342-ae25-c5f21ea06a0d)
<p>두 유저가 번갈아가며 돌을 놓고 오목이 완성되면 게임 종료 메시지와 함께 로비 돌아가기 버튼 생성</p>

### 개선 사항
- UTF-16 인코딩 수정
- send/recv 버퍼 관리

